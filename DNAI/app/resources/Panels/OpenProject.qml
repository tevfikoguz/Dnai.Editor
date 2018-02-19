import QtQuick 2.4
import QtQuick.Controls 2.2
import DNAI 1.0

import "../Forms"

OpenProjectForm {
    property Popup popup

//    popup.onOpened: function() {
//        Manager.updateCurentUserFiles();
//    }

    Connections {
        target: Manager

        onUserChanged: {
            isConnected = true;
            filesData.clear();
            for (var i = 0; i < Manager.user.files.length; ++i) {
                filesData.append(Manager.user.files[i]);
            }
        }
    }

    fileDialog.folder: Qt.resolvedUrl(StandardPath.writableLocation((StandardPath.HomeLocation)))
    fileDialog.onAccepted: {
        projectPath.text = fileDialog.fileUrl;
        console.log("You chose: " + fileDialog.fileUrl);
        var data = Manager.loadProjectData(fileDialog.fileUrl);
        viewData.clear();
        viewData.append({"field": "Nom", "value": data.name});
        viewData.append({"field": "Description", "value": data.description});
        viewData.append({"field": "Nombre de contexts", "value": data.count.contexts.toString()});
        viewData.append({"field": "Nombre de classes", "value": data.count.classes.toString()});
        viewData.append({"field": "Nombre de nodes", "value": data.count.nodes.toString()});
        viewData.append({"field": "Nombre de functions", "value": data.count.functions.toString()});
        viewData.append({"field": "Nombre de variables", "value": data.count.variables.toString()});
        viewData.append({"field": "Nombre de inputs", "value": data.count.inputs.toString()});
        viewData.append({"field": "Nombre de outputs", "value": data.count.outputs.toString()});
        viewData.append({"field": "Nombre de flows", "value": data.count.flows.toString()});
    }
    fileDialog.onRejected: {
        console.log("Canceled")
    }
    chooseButton.onClicked: {
        fileDialog.open()
    }
    openButton.onClicked: function () {
        if (projectPath.text) {
            Manager.openProject(projectPath.text);
            viewData.clear();
            projectPath.text = "";
            popup.close();
        }
    }
    cancelButton.onClicked: function () {
        viewData.clear();
        projectPath.text = "";
        popup.close();
    }
}
