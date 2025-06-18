#include <QApplication>
#include <QStyleFactory>

#include "mainwindow.h"

// toDo: Move to stylesheet location
QString styleSheet = R"(
    /* General Application */
    QMainWindow {
        background-color: #2D2D2D; /* Dark background */
        color: #E0E0E0; /* Light text color for contrast */
    }

    /* Toolbar */
    QToolBar {
        background-color: #333333; /* Dark toolbar background */
        /* border: 0.2px solid #444444; Slight border to separate  */
    }

    QToolBar::handle {
        background-color: #555555; /* Handle color for dragging */
    }

    QToolBar::separator {
        background-color: #555555;
        margin: 2px;
    }

    QToolButton {
        background-color: transparent;
        color: #E0E0E0;
        border: none;
        padding: 5px;
        font-size: 11px;
        font-weight: 500;
        text-align: center;
        qproperty-iconSize: 32px 32px;
        min-width: 64px;
        min-height: 64px;
    }
    QToolButton > * {
        qproperty-wordWrap: true;
    }
    QToolButton::menu-indicator {
        image: none;
    }
    QToolButton:hover {
        background-color: #4CAF50;
        color: white;
    }
    QToolButton:checked {
        background-color: #4CAF50;
        color: white;
    }

    /* Menu Bar */
    QMenuBar {
        background-color: #333333; /* Dark background */
        color: #E0E0E0; /* Light text color */
    }

    QMenuBar::item {
        background-color: transparent;
        padding: 5px 10px;
    }

    QMenuBar::item:selected {
        background-color: #4CAF50; /* Highlighted item */
        color: white;
    }

    /* Menu */
    QMenu {
        background-color: #333333;
        color: #E0E0E0;
        border: 1px solid #444444;
    }

    QMenu::item:selected {
        background-color: #4CAF50; /* Item highlighted on hover */
        color: white;
    }

    QMenu::item {
        padding: 8px 16px;
    }

    QToolButton {
        border: 1px solid black;
        width: 100%;
    }

    QLabel {
        color: white;
    }

    QCheckBox {
        color: white;
    }
)";

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // Set the Fusion style for a more modern look
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    qApp->setStyleSheet(styleSheet);

    MainWindow w;
    w.show();
    return a.exec();
}
