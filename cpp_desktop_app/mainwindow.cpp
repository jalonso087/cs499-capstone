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
    QVBoxLayout *listingLayout = new QVBoxLayout(tripListingPage);

    //init filter UI
    minPriceField = new QLineEdit(this);
    minPriceField->setPlaceholderText("Min Price");

    maxPriceField = new QLineEdit(this);
    maxPriceField->setPlaceholderText("Max Price");

    //instantiate filter button
    filterButton = new QPushButton("Apply Filter", this);

    //add to horizontal layout similar to a toolbar
    QHBoxLayout *filterLayout = new QHBoxLayout();
    filterLayout->addWidget(new QLabel("Filter by Price: $"));
    filterLayout->addWidget(minPriceField);
    filterLayout->addWidget(new QLabel(" to $"));
    filterLayout->addWidget(maxPriceField);
    filterLayout->addWidget(filterButton);

    //add toolbar to listing layout
    listingLayout->addLayout(filterLayout);

    //connect button
    connect(filterButton, &QPushButton::clicked, this, &MainWindow::applyPriceFilter);
    
    //add nav button to trip listing page
    QPushButton *navToAddButton = new QPushButton("Add New Trip", this);
    listingLayout->addWidget(navToAddButton);
    
    //listingLayout = new QVBoxLayout(tripListingPage);

    /*adding the navigation button to the trip listing page
    QPushButton *navToAddButton = new QPushButton("Add New Trip", this);
    listingLayout->addWidget(navToAddButton);*/

    //adding logout button
    logoutButton = new QPushButton("Logout", this);
    listingLayout->addWidget(logoutButton);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::logout);

    //Commenting out as part of debugging
    //API Test button to fetch trips from the database
    //fetchButton = new QPushButton("Test API Connection", this);
    //listingLayout->addWidget(fetchButton);
    
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
    //connect(fetchButton, &QPushButton::clicked, this, &MainWindow::fetchTrips);
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
    if (reply->error() == QNetworkReply::NoError) {
        QByteArray responseData = reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(responseData);
        QJsonArray jsonArray = jsonResponse.array();

        //clearing the tree before reloading to prevent duplicates
        tripTree.clear();

        QList<QJsonObject> initialTrips;

        //populate the BST and initial list
        for (const QJsonValue &value : jsonArray) {
            QJsonObject tripObj = value.toObject();

            //pull price string and convert to double
            QString priceString = tripObj["perPerson"].toString();
            double price = priceString.toDouble();

            tripTree.insert(price, tripObj);

            initialTrips.append(tripObj);
        }

        //render UI with all trips initially
        renderTripCards(initialTrips);
    }
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

void MainWindow::renderTripCards(const QList<QJsonObject>& trips) {
    
    //vertical card container
    QWidget *listContainer = new QWidget();
    QVBoxLayout *listLayout = new QVBoxLayout(listContainer);

    //loop through trips
    for (const QJsonObject &trip : trips) {
        QFrame *card = new QFrame();
        card->setFrameShape(QFrame::StyledPanel);
        card->setStyleSheet("QFrame { background-color: #f0f0f0; border: 1px solid #ccc; border-radius: 5px; margin: 5px; }");
        QVBoxLayout *cardLayout = new QVBoxLayout(card);

        QLabel *imagePlaceholder = new QLabel("Loading image...");
        imagePlaceholder->setAlignment(Qt::AlignCenter);
        loadTripImage(trip["image"].toString(), imagePlaceholder);

        QLabel *title = new QLabel(trip["name"].toString());
        QFont titleFont = title->font();
        titleFont.setBold(true);
        titleFont.setPointSize(12);
        title->setFont(titleFont);

        QLabel *resort = new QLabel(trip["resort"].toString());
        resort->setStyleSheet("color: gray;");

        QLabel *price = new QLabel("Price: $" + trip["perPerson"].toString());
        QLabel *length = new QLabel("Length: " + trip["length"].toString());
        
        QLabel *description = new QLabel("Description: " + trip["description"].toString());
        description->setTextFormat(Qt::RichText);
        description->setWordWrap(true);

        cardLayout->addWidget(imagePlaceholder);
        cardLayout->addWidget(title);
        cardLayout->addWidget(resort);
        cardLayout->addWidget(price);
        cardLayout->addWidget(length);
        cardLayout->addWidget(description);
        
        QPushButton *editButton = new QPushButton("Edit Trip");
        cardLayout->addWidget(editButton);

        connect(editButton, &QPushButton::clicked, this, [this, trip]() {
            openEditForm(trip);
        });

        listLayout->addWidget(card);
    }

    //create a scroll area to allow the user to scroll through the trips
    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(listContainer);

    //clear prev. scroll area if not null
    QLayout *dashboardLayout = tripListingPage->layout();
    
    //adjust the index based on how many items are showing
    if (dashboardLayout->count() > 3) { 
        QLayoutItem *existingItem = dashboardLayout->takeAt(3);
        if (existingItem != nullptr) {
            if(existingItem->widget() != nullptr) {
                existingItem->widget()->deleteLater();
            }
            delete existingItem;
        }
    }

    //new scroll area with cards
    dashboardLayout->addWidget(scrollArea);
}

void MainWindow::applyPriceFilter() {
    double minPrice = minPriceField->text().isEmpty() ? 0.0 : minPriceField->text().toDouble();
    double maxPrice = maxPriceField->text().isEmpty() ? 99999.9 : maxPriceField->text().toDouble();

    QList<QJsonObject> filteredTrips = tripTree.filterByPrice(minPrice, maxPrice);

    //render only the filtered cards
    renderTripCards(filteredTrips);
}