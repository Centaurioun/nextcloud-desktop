import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

import Style 1.0

import com.nextcloud.desktopclient 1.0 as NC

RowLayout {
    id: root

    property alias model: syncStatus

    spacing: Style.trayHorizontalMargin

    NC.SyncStatusSummary {
        id: syncStatus
    }

    NCBusyIndicator {
        id: syncIcon
        property int size: Style.trayListItemIconSize * 0.6
        property int whiteSpace: (Style.trayListItemIconSize - size)

        Layout.preferredWidth: size
        Layout.preferredHeight: size

        Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
        Layout.topMargin: 16
        Layout.rightMargin: whiteSpace * (0.5 + Style.thumbnailImageSizeReduction)
        Layout.bottomMargin: 16
        Layout.leftMargin: Style.trayHorizontalMargin + (whiteSpace * (0.5 - Style.thumbnailImageSizeReduction))

        padding: 0

        imageSource: syncStatus.syncIcon
        running: syncStatus.syncing
    }

    ColumnLayout {
        id: syncProgressLayout

        Layout.alignment: Qt.AlignVCenter
        Layout.topMargin: 8
        Layout.rightMargin: Style.trayHorizontalMargin
        Layout.bottomMargin: 8
        Layout.fillWidth: true
        Layout.fillHeight: true

        Text {
            id: syncProgressText

            Layout.fillWidth: true

            text: syncStatus.syncStatusString
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Style.topLinePixelSize
            font.bold: true
            color: Style.ncTextColor
        }

        Loader {
            Layout.fillWidth: true

            active: syncStatus.syncing
            visible: syncStatus.syncing

            sourceComponent: ProgressBar {
                id: syncProgressBar

                // TODO: Rather than setting all these palette colours manually,
                // create a custom style and do it for all components globally
                palette {
                    text: Style.ncTextColor
                    windowText: Style.ncTextColor
                    buttonText: Style.ncTextColor
                    light: Style.lightHover
                    midlight: Style.lightHover
                    mid: Style.ncSecondaryTextColor
                    dark: Style.menuBorder
                    button: Style.menuBorder
                    window: Style.backgroundColor
                    base: Style.backgroundColor
                }

                value: syncStatus.syncProgress
            }
        }

        Text {
            id: syncProgressDetailText

            Layout.fillWidth: true

            text: syncStatus.syncStatusDetailString
            visible: syncStatus.syncStatusDetailString !== ""
            color: Style.ncSecondaryTextColor
            font.pixelSize: Style.subLinePixelSize
        }
    }

    CustomButton {
        FontMetrics {
            id: syncNowFm
            font.bold: true
        }

        Layout.preferredWidth: syncNowFm.boundingRect(text).width + leftPadding + rightPadding
        Layout.rightMargin: Style.trayHorizontalMargin

        FontMetrics { font.bold: true }

        text: qsTr("Sync now")
        textColor: Style.adjustedCurrentUserHeaderColor
        textColorHovered: Style.currentUserHeaderTextColor
        bold: true
        bgColor: Style.currentUserHeaderColor

        visible: !syncStatus.syncing && NC.UserModel.currentUser.hasLocalFolder
        enabled: visible
        onClicked: {
            if(!syncStatus.syncing) {
                NC.UserModel.currentUser.forceSyncNow();
            }
        }
    }
}
