#include "mainwindow.h"
#include <QVBoxLayout>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QPixmap>
#include <QByteArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkReply>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    //initialize stack
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);
    
    //build page for trip listing view
    tripListingPage = new QWidget();
    //QVBoxLayout *listingLayout = new QVBoxLayout(tripListingPage);
    listingLayout = new QVBoxLayout(tripListingPage);

    //adding the navigation button to the trip listing page
    QPushButton *navToAddButton = new QPushButton("Add New Trip", this);
    listingLayout->addWidget(navToAddButton);

    //adding logout button
    logoutButton = new QPushButton("Logout", this);
    listingLayout->addWidget(logoutButton);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::logout);

    //API Test button to fetch trips from the database
    fetchButton = new QPushButton("Test API Connection", this);
    listingLayout->addWidget(fetchButton);
    
    //insert existing QScrollArea code here

    //build page add trip form view
    addTripPage = new QWidget();
    QFormLayout *formLayout = new QFormLayout(addTripPage);

    //build page login view
    loginPage = new QWidget();
    QFormLayout *loginLayout = new QFormLayout(loginPage);


    emailField = new QLineEdit(this);
    passwordField = new QLineEdit(this);
    //mask the password with asterisks for security
    passwordField->setEchoMode(QLineEdit::Password);

    loginSubmitButton = new QPushButton("Sign In", this);

    loginLayout->addRow("Email:", emailField);
    loginLayout->addRow("Password:", passwordField);
    loginLayout->addRow(loginSubmitButton);

    //add to stack and connect
    stackedWidget->addWidget(loginPage);
    connect(loginSubmitButton, &QPushButton::clicked, this, &MainWindow::submitLogin);

    //instantiate the input fields
    codeField = new QLineEdit(this);
    nameField = new QLineEdit(this);
    lengthField = new QLineEdit(this);
    startField = new QLineEdit(this);
    resortField = new QLineEdit(this);
    priceField = new QLineEdit(this);
    imageField = new QLineEdit(this);
    descriptionField = new QTextEdit(this);

    //add the input fields to the form layout
    formLayout->addRow("Trip Code (e.g. MEGR220119):", codeField);
    formLayout->addRow("Trip Name:", nameField);
    formLayout->addRow("Length (e.g 6 nights / 7 days):", lengthField);
    formLayout->addRow("Start Date (e.g. 2024-01-19):", startField);
    formLayout->addRow("Resort:", resortField);
    formLayout->addRow("Price per Person:", priceField);
    formLayout->addRow("Image Name (e.g reef3.jpg):", imageField);
    formLayout->addRow("Description:", descriptionField);

    //add buttons at the bottom of the form
    saveTripButton = new QPushButton("Save Trip to Database", this);
    QPushButton *backButton = new QPushButton("Cancel and return to Trips", this);
    formLayout->addRow(saveTripButton);
    formLayout->addRow(backButton);

    //back button
    //QPushButton *backButton = new QPushButton("Back to Trips", this);
    //pformLayout->addWidget(backButton);

    //add both pages to the stack
    stackedWidget->addWidget(tripListingPage);
    stackedWidget->addWidget(addTripPage);

    //navigation connections
    connect(navToAddButton, &QPushButton::clicked, this, &MainWindow::showAddTripForm);
    connect(backButton, &QPushButton::clicked, this, &MainWindow::showTripListing);

    //network management
    networkManager = new QNetworkAccessManager(this);
    connect(fetchButton, &QPushButton::clicked, this, &MainWindow::fetchTrips);
    connect(saveTripButton, &QPushButton::clicked, this, &MainWindow::submitNewTrip);
}

void MainWindow::fetchTrips() {

    QUrl apiEndpoint("http://localhost:3000/api/trips");
    QNetworkRequest request(apiEndpoint);

    QNetworkReply *reply = networkManager->get(request);

    //Connect only this json reply to parser
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        onNetworkReply(reply);
    });
}

void MainWindow::showAddTripForm() {
    //route using the  addTripPage pointer to the stacked widget to show the add trip form page
    currentEditTripCode = ""; //reset the current edit trip code when showing the add trip form
    codeField->setReadOnly(false); //allow user to edit the trip code when adding
    stackedWidget->setCurrentWidget(addTripPage);
}

void MainWindow::showTripListing() {
    
    // route using the tripListingPage pointer to the stacked widget to show the trip listing page
    stackedWidget->setCurrentWidget(tripListingPage);
}

void MainWindow::submitNewTrip() {

    //grab data from UI fields
    QJsonObject json;
    json["code"] = codeField->text();
    json["name"] = nameField->text();
    json["length"] = lengthField->text();
    json["start"] = startField->text();
    json["resort"] = resortField->text();
    json["perPerson"] = priceField->text();
    json["image"] = imageField->text();
    json["description"] = descriptionField->toPlainText();

    //convert JSON to byte array for sending
    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setRawHeader("Authorization", "Bearer " + authToken.toUtf8());

    QNetworkReply *reply;
    if (currentEditTripCode.isEmpty()) {
        //use a POST request to add a new trip
        request.setUrl(QUrl("http://localhost:3000/api/trips"));
        reply = networkManager->post(request, data);
    } else {
        //use a PUT request to update an existing trip
        request.setUrl(QUrl("http://localhost:3000/api/trips/" + currentEditTripCode));
        reply = networkManager->put(request, data);
    }

    //handle server response using with a  lambda function
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            //successful response
            qDebug() << "Trip added successfully:" << reply->readAll();

            //clear the form fields for the next entry
            currentEditTripCode = "";
            codeField->clear();
            nameField->clear();
            lengthField->clear();
            startField->clear();
            resortField->clear();
            priceField->clear();
            imageField->clear();
            descriptionField->clear();

            //navigate back to the trip listing page
            showTripListing();

            //refresh the trip listing to show the newly added trip
            fetchTrips();
        } else {
            // failure response
            qDebug() << "Error adding trip:" << reply->errorString();
        }
        //clean up the reply to prevent memory leaks
        reply->deleteLater();
    });
}

void MainWindow::loadTripImage(const QString& imageName, QLabel* imageLabel) {
    QUrl imageUrl("http://localhost:3000/images/" + imageName);
    QNetworkRequest request(imageUrl);

    QNetworkReply *reply = networkManager->get(request);

    //lambda function to handle the image loading once the network reply is finished
    connect(reply, &QNetworkReply::finished, this, [reply, imageLabel]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray imageData = reply->readAll();
            QPixmap pixmap;

            //load bytes into a pixel map
            if (pixmap.loadFromData(imageData)) {
                //scale the image to fit the card layout
                imageLabel->setPixmap(pixmap.scaled(250,150, Qt::KeepAspectRatio, Qt::SmoothTransformation));

                //clear previous placeholder styling
                imageLabel->setStyleSheet("");
            }
        } else {
            // Handle image loading error
            imageLabel->setText("Image Failed to load");
        }
        // Clean up the reply object to prevent memory leaks
        reply->deleteLater();
    });
}

void MainWindow::onNetworkReply(QNetworkReply *reply) {
    if(reply->error() == QNetworkReply::NoError) {
        // Successful response
        //parse the JSON response into an array
        QJsonDocument jsonResponse = QJsonDocument::fromJson(reply->readAll());
        QJsonArray jsonArray = jsonResponse.array();

        //Create a container to hold cards vertically
        QWidget *listContainer = new QWidget();
        QVBoxLayout *listLayout = new QVBoxLayout(listContainer);

        //Loop through the database records
        for(const QJsonValue &value : jsonArray) {
            QJsonObject trip = value.toObject();

            //Create a card for each trip
            QFrame *card = new QFrame();
            card->setFrameShape(QFrame::StyledPanel);
            card->setStyleSheet("QFrame { background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 5px; margin: 5px; }");
            QVBoxLayout *cardLayout = new QVBoxLayout(card);

            //placeholder images
            QLabel *imagePlaceholder = new QLabel("Loading image...");
            imagePlaceholder->setAlignment(Qt::AlignCenter);

            //start the download for this card's image
            loadTripImage(trip["image"].toString(), imagePlaceholder);

            //Title
            QLabel *title = new QLabel(trip["name"].toString());
            QFont titleFont = title->font();
            titleFont.setBold(true);
            titleFont.setPointSize(12);
            title->setFont(titleFont);

            //Resort and Stars
            QLabel *resort = new QLabel(trip["resort"].toString());
            resort->setStyleSheet("color: gray;");

            //Price
            QLabel *price = new QLabel("Price: $" + trip["perPerson"].toString());

            //Length
            QLabel *length = new QLabel("Length: " + trip["length"].toString());

            //Description
            QLabel *description = new QLabel("Description: " + trip["description"].toString());
            description->setTextFormat(Qt::RichText);
            description->setWordWrap(true);

            //build the cards
            cardLayout->addWidget(imagePlaceholder);
            cardLayout->addWidget(title);
            cardLayout->addWidget(resort);
            cardLayout->addWidget(price);
            cardLayout->addWidget(length);
            cardLayout->addWidget(description);
            
            //add edit button
            QPushButton *editButton = new QPushButton("Edit Trip");
            cardLayout->addWidget(editButton);

            //connect edit button to open form and pass the trip data for editing
            connect(editButton, &QPushButton::clicked, this, [this, trip]() {
                openEditForm(trip);
            });

            //add card to list
            listLayout->addWidget(card);
        }

        //Creating scroll area to allow scrolling through the cards
        QScrollArea *scrollArea = new QScrollArea;
        scrollArea->setWidgetResizable(true);
        scrollArea->setWidget(listContainer);

        //clear the previous layout and add the new scroll area with cards
        QLayoutItem *existingItem = listingLayout->itemAt(2);
        if(existingItem != nullptr) {
            delete existingItem->widget();
        }

        //add scroll area to existing layout
        listingLayout->addWidget(scrollArea);
    } else {
        // Handle error
        logConsole->append("Error: " + reply->errorString());
    }
    // this will prevent memory leaks by deleting the reply object after processing
    reply->deleteLater();
}

void MainWindow::submitLogin() {
    QJsonObject json;
    json["email"] = emailField->text();
    json["password"] = passwordField->text();

    QJsonDocument doc(json);
    QByteArray data = doc.toJson();

    //point to the login endpoint
    QUrl apiEndpoint("http://localhost:3000/api/login");
    QNetworkRequest request(apiEndpoint);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    //Send the POST request to the login endpoint
    QNetworkReply *reply = networkManager->post(request, data);

    //Handle the token response
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {

            //parse the returned JSON to extract the token
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
            QJsonObject responseObj = jsonResponse.object();

            //store the token in the class variable
            if (responseObj.contains("token")) {
                authToken = responseObj["token"].toString();
                qDebug() << "Login successful. Token saved.";

                //navigate to the main dashboard
                stackedWidget->setCurrentWidget(tripListingPage);
                fetchTrips();
            }
        } else {
            // Handle login error
            qDebug() << "Login failed:" << reply->errorString();
        }
        //to prevent memory leaks, delete the reply object after processing
        reply->deleteLater();
    });
}

void MainWindow::logout() {
    //clear auth token
    authToken = "";
    //route back to the login screen
    stackedWidget->setCurrentWidget(loginPage);
}

void MainWindow::openEditForm(const QJsonObject &tripData) {
    //save the trip code for editing the trip
    currentEditTripCode = tripData["code"].toString();

    //prefill the form fields
    codeField->setText(tripData["code"].toString());
    //prevent user from editing the trip code
    codeField->setReadOnly(true); 
    nameField->setText(tripData["name"].toString());
    lengthField->setText(tripData["length"].toString());
    startField->setText(tripData["start"].toString());
    resortField->setText(tripData["resort"].toString());
    priceField->setText(tripData["perPerson"].toString());
    imageField->setText(tripData["image"].toString());
    descriptionField->setText(tripData["description"].toString());

    //switch screen to the form
    stackedWidget->setCurrentWidget(addTripPage);
}