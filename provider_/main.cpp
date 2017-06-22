/******************************************************************************
 *    Copyright (c) Open Connectivity Foundation (OCF), AllJoyn Open Source
 *    Project (AJOSP) Contributors and others.
 *
 *    SPDX-License-Identifier: Apache-2.0
 *
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Copyright (c) Open Connectivity Foundation and Contributors to AllSeen
 *    Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for
 *    any purpose with or without fee is hereby granted, provided that the
 *    above copyright notice and this permission notice appear in all
 *    copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 *    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 *    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 *    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 *    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 *    PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 *    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *    PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

#include <cstdio>
#include <iostream>
#include <vector>
#include <memory>
#include <stdlib.h>
#include <errno.h>
#include <string>

#include <alljoyn/Status.h>
#include <alljoyn/AboutObj.h>
#include <alljoyn/BusAttachment.h>
#include <alljoyn/Init.h>

#define INTF_NAME "com.example.Door"


//static const char* UI_FILE;

using namespace std;
using namespace ajn;
string UI_FILE;
class Door;
vector<Door*> g_doors;
vector<bool> g_doors_registered;
int g_turn = 0;
SessionPort port = 123;

class SPL : public SessionPortListener {
    virtual bool AcceptSessionJoiner(SessionPort sessionPort, const char* joiner, const SessionOpts& opts) {
        QCC_UNUSED(sessionPort);
        QCC_UNUSED(joiner);
        QCC_UNUSED(opts);
        return true;
    }
};

SPL g_session_port_listener;

static QStatus BuildInterface(BusAttachment& bus)
{
    QStatus status;

    InterfaceDescription* intf = NULL;
    status = bus.CreateInterface(INTF_NAME, intf);
    QCC_ASSERT(ER_OK == status);
    status = intf->AddProperty("IsOpen", "b", PROP_ACCESS_READ);
    QCC_ASSERT(ER_OK == status);
    status = intf->AddPropertyAnnotation("IsOpen", "org.freedesktop.DBus.Property.EmitsChangedSignal", "true");
    QCC_ASSERT(ER_OK == status);
    status = intf->AddProperty("Location", "s", PROP_ACCESS_READ);
    QCC_ASSERT(ER_OK == status);
    status = intf->AddPropertyAnnotation("Location", "org.freedesktop.DBus.Property.EmitsChangedSignal", "true");
    QCC_ASSERT(ER_OK == status);
    status = intf->AddProperty("KeyCode", "u", PROP_ACCESS_READ);
    QCC_ASSERT(ER_OK == status);
    status = intf->AddPropertyAnnotation("KeyCode", "org.freedesktop.DBus.Property.EmitsChangedSignal", "invalidates");
    QCC_ASSERT(ER_OK == status);

    status = intf->AddMethod("Open", "", "", "");
    QCC_ASSERT(ER_OK == status);
    status = intf->AddMethod("Close", "", "", "");
    QCC_ASSERT(ER_OK == status);
    status = intf->AddMethod("KnockAndRun", "", "", "", MEMBER_ANNOTATE_NO_REPLY);
    QCC_ASSERT(ER_OK == status);

    status = intf->AddMethod("GetUI", "", "s", "outStr", 0);
    QCC_ASSERT(ER_OK == status);

    status = intf->AddMethod("CallFunc", "s", "s", "choice", 0);
    QCC_ASSERT(ER_OK == status);

    status = intf->AddSignal("PersonPassedThrough", "s", "name", MEMBER_ANNOTATE_SESSIONCAST);
    QCC_ASSERT(ER_OK == status);

    intf->Activate();

    return status;
}

static QStatus SetupBusAttachment(BusAttachment& bus, AboutData& aboutData)
{
    QStatus status;
    status = bus.Start();
    QCC_ASSERT(ER_OK == status);
    status = bus.Connect();
    if (status != ER_OK) {
        return status;
    }

    status = BuildInterface(bus);
    QCC_ASSERT(ER_OK == status);

    SessionOpts opts(SessionOpts::TRAFFIC_MESSAGES, false, SessionOpts::PROXIMITY_ANY, TRANSPORT_ANY);
    bus.BindSessionPort(port, opts, g_session_port_listener);

    /* set up totally uninteresting about data */
    //AppId is a 128bit uuid
    uint8_t appId[] = { 0x01, 0xB3, 0xBA, 0x14,
                        0x1E, 0x82, 0x11, 0xE4,
                        0x86, 0x51, 0xD1, 0x56,
                        0x1D, 0x5D, 0x46, 0xB0 };
    aboutData.SetAppId(appId, 16);
    aboutData.SetDeviceName("Foobar 2000 Door Security");
    //DeviceId is a string encoded 128bit UUID
    aboutData.SetDeviceId("93c06771-c725-48c2-b1ff-6a2a59d445b8");
    aboutData.SetAppName("Application");
    aboutData.SetManufacturer("Manufacturer");
    aboutData.SetModelNumber("123456");
    aboutData.SetDescription("A poetic description of this application");
    aboutData.SetDateOfManufacture("2014-03-24");
    aboutData.SetSoftwareVersion("0.1.2");
    aboutData.SetHardwareVersion("0.0.1");
    aboutData.SetSupportUrl("http://www.example.org");
    if (!aboutData.IsValid()) {
        cerr << "Invalid about data." << endl;
        return ER_FAIL;
    }
    return status;
}

static void UI_constructor(int ready, string in){
      string result;

    if (ready == 0){
        string r("import QtQuick 2.5; import QtQuick.Window 2.2; import QtQuick.Controls 1.4; Window { visible: true; width: 400; height: 500; title: qsTr(\"Кофемашина\"); color: \"maroon\"; Rectangle { id: panel1; width: parent.width*0.9; height: parent.height*0.9; color: \"white\"; anchors.centerIn: parent; opacity: 0.8; Text { id: message; color: \"black\"; text: \"Выберите напиток!\"; font.family: \"Helvetica\"; font.pixelSize: 25; anchors.horizontalCenter: parent.horizontalCenter; y:panel1.height*0.06 } Image { anchors.centerIn: panel1; source: \"coff.png\"; } MouseArea { id: mouseArea; anchors.rightMargin: 0; anchors.bottomMargin: 0; anchors.leftMargin: 0; anchors.topMargin: 0; anchors.fill: parent; Button { id: button1; onClicked: { _Handler.callFunc(\"Эспресо\"); } x:panel1.width*0.1; y:panel1.height*0.3; width: 130; height: 40; text: qsTr(\"Эспресо\") } Button { id: button2; onClicked: { _Handler.callFunc(\"Латте\"); } x:panel1.width*0.1; y:panel1.height*0.5; width: 130; height: 40; text: qsTr(\"Латте\") } Button { id: button3; onClicked: { _Handler.callFunc(\"Американо\"); } x:panel1.width*0.1; y:panel1.height*0.7; width: 130; height: 40; text: qsTr(\"Американо\") } Button { id: button4; onClicked: { _Handler.callFunc(\"Капучино\"); } x:panel1.width*0.5; y:panel1.height*0.3; width: 130; height: 40; text: qsTr(\"Капучино\") } Button { id: button5; onClicked: { _Handler.callFunc(\"Марокино\"); } x:panel1.width*0.5; y:panel1.height*0.5; width: 130; height: 40; text: qsTr(\"Марокино\") } Button { id: button6; onClicked: { _Handler.callFunc(\"Шоколад\"); } x:panel1.width*0.5; y:panel1.height*0.7; width: 130; height: 40; text: qsTr(\"Шоколад\") } } } }");
        result = r;
    }
    else {

        //char in1 = in.c_str();
        result = "import QtQuick 2.5; import QtQuick.Window 2.2; import QtQuick.Controls 1.4; Window { visible: true; width: 400; height: 500; title: qsTr(\"Кофемашина\"); color: \"maroon\"; Rectangle { id: panel1; width: parent.width*0.9; height: parent.height*0.9; color: \"white\"; anchors.centerIn: parent; opacity: 0.8; Text { id: message; color: \"black\"; text: \"Выберите напиток!\"; font.family: \"Helvetica\"; font.pixelSize: 25; anchors.horizontalCenter: parent.horizontalCenter; y:panel1.height*0.06 } Image { anchors.centerIn: panel1; source: \"coff.png\"; } MouseArea { id: mouseArea; anchors.rightMargin: 0; anchors.bottomMargin: 0; anchors.leftMargin: 0; anchors.topMargin: 0; anchors.fill: parent; Button { id: button1; onClicked: { _Handler.callFunc(\"Эспресо\"); } x:panel1.width*0.1; y:panel1.height*0.3; width: 130; height: 40; text: qsTr(\"Эспресо\") } Button { id: button2; onClicked: { _Handler.callFunc(\"Латте\"); } x:panel1.width*0.1; y:panel1.height*0.5; width: 130; height: 40; text: qsTr(\"Латте\") } Button { id: button3; onClicked: { _Handler.callFunc(\"Американо\"); } x:panel1.width*0.1; y:panel1.height*0.7; width: 130; height: 40; text: qsTr(\"Американо\") } Button { id: button4; onClicked: { _Handler.callFunc(\"Капучино\"); } x:panel1.width*0.5; y:panel1.height*0.3; width: 130; height: 40; text: qsTr(\"Капучино\") } Button { id: button5; onClicked: { _Handler.callFunc(\"Марокино\"); } x:panel1.width*0.5; y:panel1.height*0.5; width: 130; height: 40; text: qsTr(\"Марокино\") } Button { id: button6; onClicked: { _Handler.callFunc(\"Шоколад\"); } x:panel1.width*0.5; y:panel1.height*0.7; width: 130; height: 40; text: qsTr(\"Шоколад\") } Button { id: dlt; onClicked: { message_ready.visible = false; dlt.visible = false; } x:panel1.width*0.8; y:panel1.height*0.84; width: 20; height: 20; Image { anchors.centerIn: dlt; scale: 0.009; source: \"2.png\"; } } Text { id: message_ready; color: \"black\"; text: \"Скорее забирайте свой"
                +in+
                "!\"; font.family: \"Helvetica\"; font.pixelSize: 18; anchors.horizontalCenter: parent.horizontalCenter; y:panel1.height*0.9 } } } }";
    }
    UI_FILE = result;

}

class Door : public BusObject {
  private:
    uint32_t code;
    bool open;
    string location;

    BusAttachment& bus;

  public:
    Door(BusAttachment& bus, const string& location)
        : BusObject(("/doors/" + location).c_str()),
        code(1234),
        open(false),
        location(location),
        bus(bus)
    {
        const InterfaceDescription* intf = bus.GetInterface(INTF_NAME);
        QCC_ASSERT(intf);
        AddInterface(*intf, ANNOUNCED);

        /** Register the method handlers with the object */
        const MethodEntry methodEntries[] = {
            { intf->GetMember("Open"), static_cast<MessageReceiver::MethodHandler>(&Door::Open) },
            { intf->GetMember("Close"), static_cast<MessageReceiver::MethodHandler>(&Door::Close) },
            { intf->GetMember("KnockAndRun"), static_cast<MessageReceiver::MethodHandler>(&Door::KnockAndRun) },
            { intf->GetMember("GetUI"), static_cast<MessageReceiver::MethodHandler>(&Door::GetUI) },
            { intf->GetMember("CallFunc"), static_cast<MessageReceiver::MethodHandler>(&Door::CallFunc) },
        };
        QStatus status = AddMethodHandlers(methodEntries, sizeof(methodEntries) / sizeof(methodEntries[0]));
        if (ER_OK != status) {
            cerr << "Failed to register method handlers for Door." << endl;
        }
    }

    ~Door()
    {
    }

    /* property getters */
    QStatus Get(const char*ifcName, const char*propName, MsgArg& val)
    {
        if (strcmp(ifcName, INTF_NAME)) {
            return ER_FAIL;
        }

        if (!strcmp(propName, "IsOpen")) {
            val.Set("b", open);
        } else if (!strcmp(propName, "Location")) {
            val.Set("s", location.c_str());
        } else if (!strcmp(propName, "KeyCode")) {
            val.Set("u", code);
        } else {
            return ER_FAIL;
        }

        return ER_OK;
    }

    void Open(const InterfaceDescription::Member* member, Message& msg)
    {
        QCC_UNUSED(member);

        cout << "Door @ " << location.c_str() << " was requested to open." << endl;
        if (open) {
            cout << "\t... but it was already open." << endl;
            /* Send an errorCode */
            MethodReply(msg, ER_FAIL);
        } else {
            cout << "\t... and it was closed, so we can comply." << endl;
            open = true;
            MsgArg propval("b", open);
            EmitPropChanged(INTF_NAME, "IsOpen", propval, SESSION_ID_ALL_HOSTED);
            MethodReply(msg, NULL, (size_t)0);
        }
        cout << "[next up is " << g_doors[g_turn]->location.c_str() << "] >";
        cout.flush();
    }

    void Close(const InterfaceDescription::Member* member, Message& msg)
    {
        QCC_UNUSED(member);

        cout << "Door @ " << location.c_str() << " was requested to close." << endl;
        if (open) {
            cout << "\t... and it was open, so we can comply." << endl;
            open = false;
            MsgArg propval("b", open);
            EmitPropChanged(INTF_NAME, "IsOpen", propval, SESSION_ID_ALL_HOSTED);
            MethodReply(msg, NULL, (size_t)0);
        } else {
            cout << "\t... but it was already closed." << endl;
            /* Send an error with a description */
            MethodReply(msg, "org.allseenalliance.sample.Door.CloseError", "Could not close the door, already closed");
        }
        cout << "[next up is " << g_doors[g_turn]->location.c_str() << "] >";
        cout.flush();
    }

    void KnockAndRun(const InterfaceDescription::Member* member, Message& msg)
    {
        QCC_UNUSED(member);
        QCC_UNUSED(msg);

        if (!open) {
            // see who's there
            cout << "Someone knocked on door @ " << location.c_str() << endl;
            cout << "\t... opening door" << endl;
            open = true;
            MsgArg propval("b", open);
            EmitPropChanged(INTF_NAME, "IsOpen", propval, SESSION_ID_ALL_HOSTED);
            cout << "\t... GRRRR nobody there!!!" << endl;
            cout << "\t... slamming door shut" << endl;
            open = false;
            MsgArg propval2("b", open);
            EmitPropChanged(INTF_NAME, "IsOpen", propval2, SESSION_ID_ALL_HOSTED);
        } else {
            // door was open while knocking
            cout << "GOTCHA!!! @ " << location.c_str() << " door" << endl;
        }
        /* this method is annotated as "no-reply", so we don't send any reply, obviously */
    }

    void FlipOpen()
    {
        const char* action = open ? "Closing" : "Opening";
        cout << action << " door @ " << location.c_str() << "." << endl;
        open = !open;
        MsgArg propval("b", open);
        EmitPropChanged(INTF_NAME, "IsOpen", propval, SESSION_ID_ALL_HOSTED);
    }

    void ChangeCode()
    {
        cout << "door @ " << location.c_str() << ": change code" << endl;
        code = rand() % 10000; //code of max 4 digits
        /* KeyCode is an invalidating property, no use passing the value */
        MsgArg dummy;
        EmitPropChanged(INTF_NAME, "KeyCode", dummy, SESSION_ID_ALL_HOSTED);
    }

    string GetLocation() const {
        return location;
    }

        void GetUI(const InterfaceDescription::Member* member, Message& msg)
            {
                QCC_UNUSED(member);

                //char* nll = " ";

                UI_constructor(0,"");

                qcc::String outStr = UI_FILE;

                MsgArg outArg("s", outStr.c_str());
                QStatus status = MethodReply(msg, &outArg, 1);
                if (ER_OK != status) {
                printf("Ping: Error sending reply.\n");
                }
            }



        void CallFunc(const InterfaceDescription::Member* member, Message& msg)
            {
            QCC_UNUSED(member);
printf("where is error?");

            //qcc::String inStr1 = msg->GetArg(0)->v_string.str;
            const char* inStr1 = msg->GetArg(0)->v_string.str;
            //char *in = (char*)inStr1;
            string in1(inStr1);
            cout << "Идет приготовление вашего напитка - " << inStr1 << " !";

             printf("Идет приготовление вашего напитка");
            //char *in = inStr1.c_str();
             UI_constructor(1, in1);

             qcc::String outStr = UI_FILE;

             MsgArg outArg("s", outStr.c_str());
             QStatus status = MethodReply(msg, &outArg, 1);
             if (ER_OK != status) {
             printf("Ping: Error sending reply.\n");
            }
            }
    // only here to be able to do extra tracing
    void PersonPassedThrough(const string& who)
    {
        cout << who.c_str() << " will pass through door @ " << location.c_str() << "." << endl;
        const InterfaceDescription* intf = bus.GetInterface(INTF_NAME);
        if (intf == NULL) {
            cerr << "Failed to obtain the " << INTF_NAME <<  "interface. Unable to invoke the 'PersonPassedThrough' signal for"
                 << who.c_str() << "." << endl;
            return;
        }
        MsgArg arg("s", who.c_str());
        Signal(NULL, SESSION_ID_ALL_HOSTED, *(intf->GetMember("PersonPassedThrough")), &arg, 1);
    }
};

static void Help()
{
    cout << "q         quit" << endl;
    cout << "f         flip the open state of the door" << endl;
    cout << "p <who>   signal that <who> passed through the door" << endl;
    cout << "r         remove or reattach the door to the bus" << endl;
    cout << "n         move to the next door in the list" << endl;
    cout << "c         change the code of the door" << endl;
    cout << "h         show this help message" << endl;
}

static void Shutdown() {

//#ifdef ROUTER
    AllJoynRouterShutdown();
//#endif
    AllJoynShutdown();
}

int CDECL_CALL main(int argc, char** argv)
{

    argc = 2;
    argv[1] = "coffemachine";
    /* parse command line arguments */
//    if (argc == 1) {
//        cerr << "Usage: " << argv[0] << " location1 [location2 [... [locationN] ...]]" << endl;
//        return EXIT_FAILURE;
//    }

    if (AllJoynInit() != ER_OK) {
        printf("Ping: Error1.\n");
        exit(1);
        //return EXIT_FAILURE;
    }
//#ifdef ROUTER
    if (AllJoynRouterInit() != ER_OK) {
        AllJoynShutdown();
        printf("Ping: Error2.\n");
        exit(1);
        //return EXIT_FAILURE;
    }
//#endif

    BusAttachment* bus = NULL;
    bus = new BusAttachment("door_provider", true);
    QCC_ASSERT(bus != NULL);
    AboutData aboutData("en");
    AboutObj* aboutObj = new AboutObj(*bus);
    QCC_ASSERT(aboutObj != NULL);

    if (ER_OK != SetupBusAttachment(*bus, aboutData)) {
        delete aboutObj;
        aboutObj = NULL;
        delete bus;
        bus = NULL;
        printf("Ping: Error3.\n");
        exit(1);
        //return EXIT_FAILURE;
    }

    aboutObj->Announce(port, aboutData);

    for (int i = 1; i < argc; ++i) {
        Door* door = new Door(*bus, argv[i]);
        if (ER_OK != bus->RegisterBusObject(*door)) {
            cerr << "Door (probably with duplicate) name - " << argv[i] << " - could not be registered on bus! Skipping it..." << endl;
            delete door;
            continue;
        }
        g_doors.push_back(door);
        g_doors_registered.push_back(true);
        aboutObj->Announce(port, aboutData);
    }

    if (g_doors.empty()) {
        cerr << "No doors available" << endl;
        delete bus;
        bus = NULL;
        Shutdown();
        printf("Ping: Error4.\n");
        exit(1);
        //return EXIT_FAILURE;
    }

    //printf("Device '%s'is ready!", g_doors[g_turn]->GetLocation().c_str() );
    //cout << "Device " << g_doors[g_turn]->GetLocation().c_str() << " is ready!";
    //cout <<g_doors[g_turn]->GetLocation().c_str(); }

    bool done = false;
    while (!done) {
        printf("Device '%s'is ready!", g_doors[g_turn]->GetLocation().c_str() );
  //  }

        string input;
        getline(cin, input);
        if (input.length() == 0) {
            continue;
        }
        bool nextDoor = true;
        char cmd = input[0];
        switch (cmd) {
        case 'q': {
                done = true;
                break;
            }

//        case 'f': {
//                g_doors[g_turn]->FlipOpen();
//                break;
//            }

//        case 'c': {
//                g_doors[g_turn]->ChangeCode();
//                break;
//            }

//        case 'p': {
//                size_t whopos = input.find_first_not_of(" \t", 1);
//                if (whopos == input.npos) {
//                    Help();
//                    break;
//                }
//                string who = input.substr(whopos);
//                g_doors[g_turn]->PersonPassedThrough(who);
//                break;
//            }

//        case 'r': {
//                Door& d = *g_doors[g_turn];
//                if (g_doors_registered[g_turn]) {
//                    bus->UnregisterBusObject(d);
//                } else {
//                    bus->RegisterBusObject(d);
//                }
//                g_doors_registered[g_turn] = !g_doors_registered[g_turn];
//                aboutObj->Announce(port, aboutData);
//                break;
//            }

//        case 'n': {
//                break;
//            }

//        case 'h':
//        default: {
//                Help();
//                nextDoor = false;
//                break;
//            }
        }

        if (true == nextDoor) {
            g_turn = (g_turn + 1) % g_doors.size();
        }
    }

    delete aboutObj;
    aboutObj = NULL;
    delete bus;
    bus = NULL;

    Shutdown();
    printf("Ping: Error5.\n");
    exit(1);
    //return EXIT_SUCCESS;
}
