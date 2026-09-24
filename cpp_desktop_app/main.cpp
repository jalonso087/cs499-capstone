////////////////////////////////////////////////////////
//  
//  This is the primary file for the native desktop 
//  application to handle the admin functionality
//
//  Author: Javier Alonso
//  Date:   9/15/2026
////////////////////////////////////////////////////////  

#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.resize(600, 400);
    w.show();
    return a.exec();
}