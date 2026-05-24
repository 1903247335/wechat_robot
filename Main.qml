import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls
import "./component"

import wx.module.Searcher
import wx.module.Injector
import wx.module.Starter
import wx.module.Server
import wx.module.WxManager
Window {
    id: root
    property string state: "inject"
    //property string state:"mainPanel"
    width: 640
    height: 480
    visible: true
    title: qsTr("微信机器人")

    MyPopup {
            id: tip


        }



    Component.onCompleted: {
        //Searcher.startTimer()

       dllPath.text="E:/c++/wx/wxdll32/x64/Debug/wxdll32.dll"
       exePath.text="E:/python/ai逆向结果/WeChat/WeChat.exe"
    }




    Connections{
        target:Server
        function onChangeToScanQRcode(qrcodeUrl){

        qrCodeImage.source=qrcodeUrl
            root.state = "scanQR"
        }

        function onChangeToWaitAuth(headUrl){
            console.log(headUrl)
            loginUserAvatar.source=headUrl
        root.state = "waitAuth"

        }
        function onChangeToLoginSuccess(){
        root.state = "loginSuccess"
        }
        function onChangeToLoginFail(){
        root.state="loginFail"
        }
        function onChangeToAuthSuccess(){
        root.state="authSuccess"

        }



    }



   Column {
        spacing: 10
        anchors.centerIn: parent

        opacity: root.state === "inject" ? 1.0 : 0.0
                scale:  root.state === "inject" ? 1.0 : 0.95
        visible: opacity > 0
        Behavior on opacity {
                    NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
                }
        Behavior on scale {
                    NumberAnimation { duration: 250; easing.type: Easing.OutCubic }
                }



            RowLayout{
                height:30
                 width: 400
                spacing: 10

                FileDialog{
                nameFilters: ["EXE (*.exe)", "All files (*)"]
                    id:fileEXEDialog
                    title:"选择微信文件"
                    onAccepted: {

                        exePath.text=selectedFile.toString().replace("file:///", "")


                    }
                }

                Text{

                    text:"微信路径"
                }
                Rectangle{
                   Layout.fillWidth: true
                    Layout.fillHeight: true
                    border.color: "black"
                    border.width: 1
                    radius: 4
                    TextInput{
                        id:exePath
                        anchors.fill: parent
                        verticalAlignment: TextInput.AlignVCenter

                        leftPadding: 8
                        rightPadding: 8
                        clip: true
                    }

                    DropArea{
                        anchors.fill: parent
                        onEntered: (drag)=>{

                                   drag.accept()
                                   }
                        onDropped: (drop)=>{

                                   let str=drop.urls[0].toString()
                                        if(!str.toLowerCase().endsWith(".exe")){
                                             tip.text="必须是EXE文件"
                                       tip.open()
                                        return
                                       }

                                       exePath.text=str.replace("file:///","")

                                   }
                    }

                }
                MyButton{
                    btnText:"选择EXE"
                     onClicked:fileEXEDialog.open()
                }


            }

        RowLayout{
            height:30
             width: 400
            spacing: 10

            FileDialog{
            nameFilters: ["DLL files (*.dll)", "All files (*)"]
                id:fileDialog
                title:"选择要注入的DLL文件"
                onAccepted: {

                    dllPath.text=selectedFile.toString().replace("file:///", "")


                }
            }

            Text{

                text:"DLL路径"
            }
            Rectangle{
               Layout.fillWidth: true
                Layout.fillHeight: true
                border.color: "black"
                border.width: 1
                radius: 4
                TextInput{
                    id:dllPath
                    anchors.fill: parent
                    verticalAlignment: TextInput.AlignVCenter

                    leftPadding: 8
                    rightPadding: 8
                    clip: true
                }

                DropArea{
                    anchors.fill: parent
                    onEntered: (drag)=>{

                               drag.accept()
                               }
                    onDropped: (drop)=>{

                               let str=drop.urls[0].toString()
                                    if(!str.toLowerCase().endsWith(".dll")){
                                         tip.text="必须是DLL文件"
                                   tip.open()
                                    return
                                   }

                                   dllPath.text=str.replace("file:///","")

                               }
                }

            }
            MyButton{
                btnText:"选择DLL"
                 onClicked:fileDialog.open()
            }


        }
        RowLayout{



            MyButton{
            text:"启动微信"
            onClicked: {
                if(!dllPath.text.endsWith(".dll")&&!exePath.text.endsWith(".exe")){
                 tip.text="请填写正确dll或者exe路径"
                tip.open()
                return
                }else{


                    if( !Starter.startWeChat(exePath.text,dllPath.text)){
                         root.state="inject"
                        tip.text="启动失败"
                        tip.open()

                    }else{


                    }


                }
            }
            }


        }

   }
   Column{

    id:qrCodeArea
    spacing: 20
    anchors.centerIn: parent
    visible: opacity > 0
    opacity: root.state=="scanQR" ? 1.0 : 0.0
    scale: root.state=="scanQR" ? 1.0 : 1.05 // 出现时从略大缩小到正常，仿佛“拍”在屏幕上
    Behavior on opacity {
                // 稍微延迟 100ms 出现，等旧界面快消失时再出来，避免重叠杂乱
                NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
            }
    Behavior on scale {
                // OutBack 带有轻微的回弹效果，非常适合成功状态的展示
                NumberAnimation { duration: 400; easing.type: Easing.OutBack }
            }


    Text{
        text:"注入成功！请扫描二维码登录"
    }

    Image{
        id:qrCodeImage
        width:200
        height:200
        source: ""
        fillMode:Image.PreserveAspectFit

    }
   }

   Column {
       id: waitAuthArea
       spacing: 20
       anchors.centerIn: parent

       visible: opacity > 0
       opacity: root.state === "waitAuth" ? 1.0 : 0.0
       scale: root.state === "waitAuth" ? 1.0 : 0.95

       Behavior on opacity {
           NumberAnimation {
               duration: 300
               easing.type: Easing.OutCubic
           }
       }

       Behavior on scale {
           NumberAnimation {
               duration: 400
               easing.type: Easing.OutBack
           }
       }

       // 头像
       Rectangle {
           width: 88
           height: 88
           radius: width / 2
           color: "#f2f2f2"
           border.width: 1
           border.color: "#dddddd"

           anchors.horizontalCenter: parent.horizontalCenter
           clip: true

           Image {
               id:loginUserAvatar
               anchors.fill: parent
               source: ""
               fillMode: Image.PreserveAspectCrop
               asynchronous: true
               cache: true
           }
       }

       Text {
           text: "扫描成功"
           font.pixelSize: 20
           font.bold: true
           color: "#333333"
           anchors.horizontalCenter: parent.horizontalCenter
       }



       Text {
           text: "请在手机微信上点击确认登录"
           font.pixelSize: 14
           color: "#666666"
           anchors.horizontalCenter: parent.horizontalCenter
       }

       BusyIndicator {
           width: 48
           height: 48
           anchors.horizontalCenter: parent.horizontalCenter
           running: root.state === "waitAuth"
       }
   }

       // --- 4. 登录成功终态 (loginSuccess) ---
    Column {
           id: loginSuccessArea
           spacing: 15
           anchors.centerIn: parent

           visible: opacity > 0
           opacity: root.state === "loginSuccess" ? 1.0 : 0.0
           // 成功时放大一点点弹出，充满成就感
           scale: root.state === "loginSuccess" ? 1.0 : 1.1
           Behavior on opacity { NumberAnimation { duration: 300; easing.type: Easing.OutCubic } }
           Behavior on scale { NumberAnimation { duration: 500; easing.type: Easing.OutElastic } } // 使用弹性曲线，让成功提示“跳”出来

           Text {
               text: "✅ 登录成功"
               font.pixelSize: 24
               font.bold: true
               color: "#07C160" // 微信绿：提供安全感与成功的心理暗示
               anchors.horizontalCenter: parent.horizontalCenter
           }
           onOpacityChanged: {
                       if (opacity === 1.0) {
                           autoTransitionTimer.start() // 界面完全显现后，开始倒计时
                       }
                   }
           Text {
               text: "机器人服务正在运行中..."
               font.pixelSize: 14
               color: "#999999"
               anchors.horizontalCenter: parent.horizontalCenter
           }
           Timer{
            id:autoTransitionTimer
            interval: 2000 // 2000毫秒 = 2秒。你可以根据需要改成 1500 或 3000
                        running: false
                        repeat: false
                        onTriggered: {
                            root.state = "mainPanel" // 倒计时结束，无缝切入主控制面板
                        }

           }
       }

    Column {
               id: loginFailArea
               spacing: 20
               anchors.centerIn: parent

               visible: opacity > 0
               opacity: root.state === "loginFail" ? 1.0 : 0.0

               // 特效设计：失败时稍微缩小，结合下面的位移动画
               scale: root.state === "loginFail" ? 1.0 : 0.9

               Behavior on opacity { NumberAnimation { duration: 200 } }

               // 核心特效：左右摇晃 (Shake Animation)
               // 当状态变为 loginFail 时，触发一个震动效果
               onOpacityChanged: {
                   if (opacity === 1.0) {
                       shakeAnim.start()
                   }
               }

               SequentialAnimation {
                   id: shakeAnim
                   NumberAnimation { target: loginFailArea; property: "anchors.horizontalCenterOffset"; from: 0; to: 15; duration: 50; easing.type: Easing.OutCubic }
                   NumberAnimation { target: loginFailArea; property: "anchors.horizontalCenterOffset"; from: 15; to: -15; duration: 50; easing.type: Easing.OutCubic }
                   NumberAnimation { target: loginFailArea; property: "anchors.horizontalCenterOffset"; from: -15; to: 10; duration: 50; easing.type: Easing.OutCubic }
                   NumberAnimation { target: loginFailArea; property: "anchors.horizontalCenterOffset"; from: 10; to: -10; duration: 50; easing.type: Easing.OutCubic }
                   NumberAnimation { target: loginFailArea; property: "anchors.horizontalCenterOffset"; from: -10; to: 0; duration: 50; easing.type: Easing.OutCubic }
               }

               Text {
                   text: "❌ 登录失败"
                   font.pixelSize: 22
                   font.bold: true
                   color: "#FA5151" // 警示红：直观传达错误信息
                   anchors.horizontalCenter: parent.horizontalCenter
               }

               Text {
                   text: "可能是网络超时或操作被取消"
                   font.pixelSize: 14
                   color: "#666666"
                   anchors.horizontalCenter: parent.horizontalCenter
               }

               // 建设性的反馈：给用户一个返回重试的按钮
               Button {
                   text: "返回重新尝试"
                   anchors.horizontalCenter: parent.horizontalCenter

                   // 简单的按钮样式自定义（如果没用 MyButton 的话）
                   contentItem: Text {
                       text: parent.text
                       color: "white"
                       horizontalAlignment: Text.AlignHCenter
                       verticalAlignment: Text.AlignVCenter
                   }

                   background: Rectangle {
                       implicitWidth: 120
                       implicitHeight: 40
                       color: parent.down ? "#d44343" : "#FA5151"
                       radius: 20 // 圆角按钮更友好
                   }

                   onClicked: {
                      root.state = "scanQR"
                   }
               }
           }

    Column {
            id: authSuccessArea
            spacing: 15
            anchors.centerIn: parent

            // 状态控制：只有在 authSuccess 状态下才可见
            visible: opacity > 0
            opacity: root.state === "authSuccess" ? 1.0 : 0.0

            // 成功时从略大（1.1）配合弹性曲线缩小回 1.0，营造一种“弹跳”落定感
            scale: root.state === "authSuccess" ? 1.0 : 1.1

            // 动画曲线保持你高水准的微动效风格
            Behavior on opacity {
                NumberAnimation { duration: 300; easing.type: Easing.OutCubic }
            }
            Behavior on scale {
                // OutElastic 带有Q弹的物理回弹效果，非常适合成功/收尾状态的展示
                NumberAnimation { duration: 600; easing.type: Easing.OutElastic }
            }

            // 大勾勾图标或文字提示
            Text {
                text: "✅"
                font.pixelSize: 48
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "认证成功"
                font.pixelSize: 22
                font.bold: true
                color: "#07C160" // 微信经典绿
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Text {
                text: "微信已成功连接，正在初始化服务..."
                font.pixelSize: 14
                color: "#888888" // 浅色辅助文本
                anchors.horizontalCenter: parent.horizontalCenter
            }


        }

    Column {
            id: mainPanelArea
            spacing: 20 // 稍微缩小间距，为输入框腾出垂直空间
            anchors.centerIn: parent
            width: parent.width * 0.85

            // 状态控制
            visible: opacity > 0
            opacity: root.state === "mainPanel" ? 1.0 : 0.0
            scale: root.state === "mainPanel" ? 1.0 : 0.95

            Behavior on opacity { NumberAnimation { duration: 250; easing.type: Easing.OutCubic } }
            Behavior on scale { NumberAnimation { duration: 300; easing.type: Easing.OutBack } }

            // 1. 顶部状态页眉
            Row {
                spacing: 12
                anchors.horizontalCenter: parent.horizontalCenter

                Rectangle {
                    width: 10; height: 10; radius: 5
                    color: "#07C160"
                    anchors.verticalCenter: parent.verticalCenter
                }
                Text {
                    text: "服务已就绪 (进程守护中)"
                    font.pixelSize: 15
                    font.bold: true
                    color: "#07C160"
                }
            }

            // ==================== 2. 核心：消息发送测试面板 ====================
            Rectangle {
                id: testSendCard
                width: parent.width
                implicitHeight: 190
                radius: 12
                color: "#F7F7F7" // 浅灰底色
                anchors.horizontalCenter: parent.horizontalCenter

                Column {
                    anchors.fill: parent
                    anchors.margins: 15
                    spacing: 12

                    Text {
                        text: "💬 消息发送功能测试"
                        font.pixelSize: 14
                        font.bold: true
                        color: "#333333"
                    }

                    // 接收人输入框
                    Row {
                        width: parent.width
                        spacing: 10
                        Text {
                            text: "接收名字:"; font.pixelSize: 13; color: "#555555"
                            width: 60; anchors.verticalCenter: parent.verticalCenter
                        }
                        TextField {
                            id: targetNameInput
                               color:"black"
                            width: parent.width - 70
                            placeholderText: "请输入微信备注或群聊名称..."
                            font.pixelSize: 13
                            selectByMouse: true
                            background: Rectangle {
                                implicitHeight: 32
                                radius: 6
                                color: "white"
                                border.color: parent.activeFocus ? "#07C160" : "#E5E5E5"
                                border.width: 1
                            }
                        }
                    }

                    // 发送内容输入框
                    Row {
                        width: parent.width
                        spacing: 10
                        Text {
                            text: "发送内容:"; font.pixelSize: 13; color: "#555555"
                            width: 60; anchors.verticalCenter: parent.verticalCenter
                        }
                        TextField {
                            id: messageContentInput
                            color:"black"
                            width: parent.width - 70
                            placeholderText: "请输入测试消息内容..."
                            font.pixelSize: 13
                            selectByMouse: true
                            background: Rectangle {
                                implicitHeight: 32
                                radius: 6
                                color: "white"
                                border.color: parent.activeFocus ? "#07C160" : "#E5E5E5"
                                border.width: 1
                            }
                        }
                    }

                    // 发送测试按钮
                    Button {
                        text: "发送测试消息"
                        anchors.right: parent.right // 靠右对齐更符合表单规范

                        contentItem: Text {
                            text: parent.text
                            color: "white"
                            font.bold: true
                            font.pixelSize: 12
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                        background: Rectangle {
                            implicitWidth: 100
                            implicitHeight: 32
                            radius: 6
                            color: parent.down ? "#06AD56" : "#07C160"
                        }
                        onClicked: {

                            // 在这里对接你的 C++ 或后台发送逻辑
                            console.log("【测试发送】目标:", targetNameInput.text, "内容:", messageContentInput.text)
  Manager.sendMessage( targetNameInput.text,messageContentInput.text,"1");
                            // 示例：触发你的底层或调用自定义信号
                            // root.triggeredTestSend(targetNameInput.text, messageContentInput.text)
                        }
                    }
                }
            }
            // =============================================================

            // 3. 底部底座控制栏
            Row {
                spacing: 15
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    text: "停止服务"
                    contentItem: Text {
                        text: parent.text
                        color: "#666666"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        implicitWidth: 100; implicitHeight: 36; radius: 18
                        color: parent.down ? "#E5E5E5" : "#F2F2F2"
                    }
                    onClicked: {
                        root.state = "scanQR"
                    }
                }

                Button {
                    text: "进入后台"
                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        font.bold: true
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    background: Rectangle {
                        implicitWidth: 100; implicitHeight: 36; radius: 18
                        color: parent.down ? "#06AD56" : "#07C160"
                    }
                    onClicked: {
                        console.log("执行进入后台逻辑")
                    }
                }
            }
        }
}
