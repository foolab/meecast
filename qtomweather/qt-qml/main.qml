//import Qt 4.7
import QtQuick 1.1
//import "." 1.0
import com.nokia.meego 1.0

PageStackWindow {
    id: rootWindow

    platformStyle: defaultStyle;

    PageStackWindowStyle { id: defaultStyle }
    PageStackWindowStyle {
        id: customStyle
        background: "image://theme/meegotouch-wallpaper-portrait";
        backgroundFillMode: Image.PreserveAspectCrop
    }
    Component.onCompleted: {
        theme.inverted = true;
    }

    initialPage: WeatherPage {}

    function openFile(file)
    {
        var component = Qt.createComponent(file);
        if (component.status == Component.Ready){
            pageStack.push(component);
        }else {
            console.log("error open file "+file);
        }
    }

    Menu {
        id: myMenu
        visualParent: pageStack
        MenuLayout {
            MenuItem {
                id: item1
                text: Config.tr("Manage locations")
                onClicked: {
                    rootWindow.openFile("StationsPage.qml");

                }
            }
            MenuItem {
                id: item2
                text: Config.tr("About")
                onClicked: {
                    rootWindow.openFile("AboutPage.qml");
                }
            }
        }
    }

}