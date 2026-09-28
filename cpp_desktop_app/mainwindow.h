#define MAINWINDOW_H

#include <QFormLayout>
#include <QLineEdit>
#include <QStackedWidget>
#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include "bst.h"

class MainWindow : public QMainWindow {
    Q_OBJECT


// Creating a nullptr for the parent widget to allow for the main window to be created without a parent
public:
    MainWindow(QWidget *parent = nullptr);

// fetchTrips() function to handle the network request to fetch trips from the server
private slots:
    void fetchTrips();
    void onNetworkReply(class QNetworkReply *reply);

    // Navigation handlers for the stacked widget to switch between different views
    void showAddTripForm();
    void showTripListing();
    void submitNewTrip();
    void logout();
    void openEditForm(const QJsonObject &tripData);
    void applyPriceFilter();

// Create pointers for network manager, fetch button, and log console to handle network requests and display logs
private:
    QNetworkAccessManager *networkManager;
    TripBST tripTree;

    //UI components for nav
    QVBoxLayout *listingLayout;
    QStackedWidget *stackedWidget;
    QWidget *tripListingPage;
    QWidget *addTripPage;
    QPushButton *fetchButton;
    QTextEdit *logConsole;
    QLineEdit *codeField;
    QLineEdit * nameField;
    QLineEdit *lengthField;
    QLineEdit *startField;
    QLineEdit *resortField;
    QLineEdit *priceField;
    QLineEdit *imageField;
    QTextEdit *descriptionField;
    QPushButton *saveTripButton;
    QWidget *loginPage;
    QLineEdit *emailField;
    QLineEdit *passwordField;
    QPushButton *loginSubmitButton;
    QString authToken;
    QPushButton *logoutButton;
    QString currentEditTripCode;
    QLineEdit *minPriceField;
    QLineEdit *maxPriceField;
    QPushButton *filterButton;
    void submitLogin();
    void loadTripImage(const QString& imageName, QLabel* imageLabel);
    void renderTripCards (const QList<QJsonObject>& trips);
};