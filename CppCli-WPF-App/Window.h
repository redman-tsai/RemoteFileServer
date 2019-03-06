#ifndef WINDOW_H
#define WINDOW_H
///////////////////////////////////////////////////////////////////////////
// Window.h - C++\CLI implementation of WPF Application                  //
//          - 687 Project #4                                             //
// ver 2.1                                                               //
// Ruizhe Cai 354520026 rcai100@syr.edu                                  //
// based on ver 2.0 by Prof. Fawcett                                     //
///////////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package demonstrates how to build a C++\CLI WPF application.  It 
*  provides one class, WPFCppCliDemo, derived from System::Windows::Window
*  that is compiled with the /clr option to run in the Common Language
*  Runtime, and another class MockChannel written in native C++ and compiled
*  as a DLL with no Common Language Runtime support.
*
*  The window class hosts, in its window, a tab control with three views, two
*  of which are provided with functionality that you may need for Project #4.
*  It loads the DLL holding MockChannel.  MockChannel hosts a send queue, a
*  receive queue, and a C++11 thread that reads from the send queue and writes
*  the deQ'd message to the receive queue.
*
*  The Client can post a message to the MockChannel's send queue.  It hosts
*  a receive thread that reads the receive queue and dispatches any message
*  read to a ListBox in the Client's FileList tab.  So this Demo simulates
*  operations you will need to execute for Project #4.
*
*  Required Files:
*  ---------------
*  Window.h, Window.cpp, MochChannel.h, MochChannel.cpp,
*  Cpp11-BlockingQueue.h, Cpp11-BlockingQueue.cpp
*
*  Build Command:
*  --------------
*  devenv CppCli-WPF-App.sln
*  - this builds C++\CLI client application and native mock channel DLL
*
*  Maintenance History:
*  --------------------
*  ver 2.1 : 3 May 2015
*  ver 2.0 : 15 Apr 2015
*  - completed message passing demo with moch channel
*  - added FileBrowserDialog to show files in a selected directory
*  ver 1.0 : 13 Apr 2015
*  - incomplete demo with GUI but not connected to mock channel
*/
/*
* Create C++/CLI Console Application
* Right-click project > Properties > Common Language Runtime Support > /clr
* Right-click project > Add > References
*   add references to :
*     System
*     System.Windows.Presentation
*     WindowsBase
*     PresentatioCore
*     PresentationFramework
*/
using namespace System;
using namespace System::Text;
using namespace System::Windows;
using namespace System::Windows::Input;
using namespace System::Windows::Markup;
using namespace System::Windows::Media;                   // TextBlock formatting
using namespace System::Windows::Controls;                // TabControl
using namespace System::Windows::Controls::Primitives;    // StatusBar
using namespace System::Threading;
using namespace System::Threading::Tasks;
using namespace System::Windows::Threading;
using namespace System::ComponentModel;


#include "../MockChannel/MockChannel.h"
#include <iostream>

namespace CppCliWindows
{
  ref class WPFCppCliDemo : Window
  {
    // MockChannel references

    ISendr* pSendr_;
    IRecvr* pRecvr_;
    IMockChannel* pChann_;

    // Controls for Window

    DockPanel^ hDockPanel = gcnew DockPanel();      // support docking statusbar at bottom
    Grid^ hGrid = gcnew Grid();                    
    TabControl^ hTabControl = gcnew TabControl();
    TabItem^ hSendMessageTab = gcnew TabItem();
    TabItem^ hFileListTab = gcnew TabItem();
    TabItem^ hConnectTab = gcnew TabItem();
	TabItem^ hQueryTab = gcnew TabItem();
	TabItem^ hPerformanceTab = gcnew TabItem();
	TabItem^ hTextSearchTab = gcnew TabItem();
	TabItem^ hXMLViewTab = gcnew TabItem();
    StatusBar^ hStatusBar = gcnew StatusBar();
    StatusBarItem^ hStatusBarItem = gcnew StatusBarItem();
    TextBlock^ hStatus = gcnew TextBlock();

    // Controls for SendMessage View

    Grid^ hSendMessageGrid = gcnew Grid();
    Button^ hSendButton = gcnew Button();
    Button^ hClearButton = gcnew Button();
    TextBlock^ hTextBlock1 = gcnew TextBlock();
	TextBox^ hMessageBox = gcnew TextBox();
    ScrollViewer^ hScrollViewer1 = gcnew ScrollViewer();
    StackPanel^ hStackPanel1 = gcnew StackPanel();

    String^ msgText 
      = "Command:ShowMessage\n"   // command
      + "Sendr:localhost@8080\n"  // send address
      + "Recvr:localhost@8090\n"  // receive address
      + "Content-length:44\n"     // body length attribute
      + "\n"                      // end header
      + "Hello World\nCSE687 - Object Oriented Design";  // message body


    // Controls for FileListView View

    Grid^ hFileListGrid = gcnew Grid();
    Forms::FolderBrowserDialog^ hFolderBrowserDialog = gcnew Forms::FolderBrowserDialog();
    ListBox^ hListBox = gcnew ListBox();
    Button^ hFolderBrowseButton = gcnew Button();
	Button^ hUploadButton = gcnew Button();
	StackPanel^ hStackPanel2 = gcnew StackPanel();
	Button^ hSetUploadFileButton = gcnew Button();
	StackPanel^ hFILEpanel1 = gcnew StackPanel();
	TextBlock^ hFN1 = gcnew TextBlock();
	TextBlock^ hFN1CT = gcnew TextBlock();

	//Controls for Query and Upload
	Grid^ hQueryListGrid = gcnew Grid;
	ListBox^ hFolders = gcnew ListBox();
	ListBox^ hFiles = gcnew ListBox();
	TextBlock^ hDIRS = gcnew TextBlock();
	TextBlock^ hFLS = gcnew TextBlock();
	CheckBox^ hifXML = gcnew CheckBox();
	TextBox^ hPatt = gcnew TextBox();
	Button^ hQuery = gcnew Button();
	Button^ hDownload = gcnew Button();
	Button^ hSetPatt = gcnew Button();
	StackPanel^ hPatternPanel = gcnew StackPanel();
	StackPanel^ hButtonPanel = gcnew StackPanel();
	StackPanel^ hAddedPatterns = gcnew StackPanel();
	TextBlock^ hPT = gcnew TextBlock();
	TextBlock^ hPatterns = gcnew TextBlock();
	Button^ hSetUploadFileButton2 = gcnew Button();
	StackPanel^ hFILEpanel2 = gcnew StackPanel();
	TextBlock^ hFN2 = gcnew TextBlock();
	TextBlock^ hFN2CT = gcnew TextBlock();
	Button^ hSelectPathButton = gcnew Button();
	StackPanel^ hSelectPathPanel = gcnew StackPanel();
	TextBlock^ hPTH = gcnew TextBlock();
	TextBlock^ hPTHCT = gcnew TextBlock();


	//Controls for Text Search
	Grid^ hTestSearchGrid = gcnew Grid;
	ListBox^ hMatchedFiles = gcnew ListBox();
	ListBox^ hMachines = gcnew ListBox();
	TextBlock^ hFlable = gcnew TextBlock();
	TextBlock^ hMlable = gcnew TextBlock();
	TextBox^ hText = gcnew TextBox();
	TextBox^ hThreadNumber = gcnew TextBox();
	CheckBox^ hTifXML = gcnew CheckBox();
	Button^ hSetThread = gcnew Button();
	Button^ hSearch = gcnew Button();
	Button^ hTDownload = gcnew Button();
	Button^ hSetText = gcnew Button();
	StackPanel^ hTextPanel = gcnew StackPanel();
	StackPanel^ hTButtonPanel = gcnew StackPanel();
	StackPanel^ hAddedTexts = gcnew StackPanel();
	StackPanel^ hTPatternPanel = gcnew StackPanel();
	TextBox^ hTPatt = gcnew TextBox();
	TextBlock^ hTT = gcnew TextBlock();
	Button^ hTSetPatt = gcnew Button();
	StackPanel^ hTAddedPatterns = gcnew StackPanel();
	TextBlock^ hTPT = gcnew TextBlock();
	TextBlock^ hTPatterns = gcnew TextBlock();
	TextBlock^ hTargetText = gcnew TextBlock();
	Button^ hSetUploadFileButton3 = gcnew Button();
	StackPanel^ hFILEpanel3 = gcnew StackPanel();
	TextBlock^ hFN3 = gcnew TextBlock();
	TextBlock^ hFN3CT = gcnew TextBlock();
	Button^ hSelectPathButton2 = gcnew Button();
	StackPanel^ hSelectPathPanel2 = gcnew StackPanel();
	TextBlock^ hPTH2 = gcnew TextBlock();
	TextBlock^ hPTHCT2 = gcnew TextBlock();
	TextBlock^ hTHN = gcnew TextBlock();
	
	//Controls for Connect Tab
	Grid^ hConnectGrid = gcnew Grid();
	Button^ hConnectButton = gcnew Button();
	TextBlock^ hConnectionHistory = gcnew TextBlock();
	TextBox^ hDstEndBox = gcnew TextBox();
	ScrollViewer^ hScrollViewerC = gcnew ScrollViewer();
	StackPanel^ hStackPanelC = gcnew StackPanel();

	//Controls for Performance
	Grid^ hPerfGrid = gcnew Grid();
	TextBlock^ hPerfHistory = gcnew TextBlock();
	ScrollViewer^ hScrollViewerPerf = gcnew ScrollViewer();

	//View for XMLview
	Grid^ hXMLGrid = gcnew Grid();
	TextBlock^ hXMLView = gcnew TextBlock();
	ScrollViewer^ hScrollViewerXML= gcnew ScrollViewer();

    // receive thread
    Thread^ recvThread;

  public:
	  WPFCppCliDemo(String^ lclport);
    ~WPFCppCliDemo();

    void setUpStatusBar();
    void setUpTabControl();
    void setUpSendMessageView();
    void setUpFileListView();
	void setUpQueryListView();
	void setUpTextSearchListView();
    void setUpConnectionView();
	void setUpPerformanceView();
	void setUpXMLView();

    void sendMessage();
	void sendTextBoxMessage(Object^ obj, RoutedEventArgs^ args);
	void addPattern(Object^ obj, RoutedEventArgs^ args);
	void addTPattern(Object^ obj, RoutedEventArgs^ args);
	void addTextSearch(Object^ obj, RoutedEventArgs^ args);
	void addUploadFilename(Object^ obj, RoutedEventArgs^ args);
	void addDownLoadFilename(Object^ obj, RoutedEventArgs^ args);
	void addDownLoadFilenameT(Object^ obj, RoutedEventArgs^ args);
	void addPath(Object^ obj, RoutedEventArgs^ args);
	void addEnds(Object^ obj, RoutedEventArgs^ args);
	void sendConnect(Object^ obj, RoutedEventArgs^ args);
	void sendUpload(Object^ obj, RoutedEventArgs^ args);
	void sendDownload(Object^ obj, RoutedEventArgs^ args);
	void sendDownloadT(Object^ obj, RoutedEventArgs^ args);
	void sendQuery(Object^ obj, RoutedEventArgs^ args);
	void sendTextQuery(Object^ obj, RoutedEventArgs^ args);
    void setThreadNumberF(Object^ obj, RoutedEventArgs^ args);

    void addText(String^ msg);
    void getMessage();
    void clear(Object^ sender, RoutedEventArgs^ args);
    void browseForFolder(Object^ sender, RoutedEventArgs^ args);
    void OnLoaded(Object^ sender, RoutedEventArgs^ args);
    void Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args);

	void QueryView(String^ reply);
	void TextSearchView(String^ reply);
	void PerformanceView(String^ reply);
	void ConnectionHistory(String^ reply);
	void XmlViewer(String^ reply);
	void filetransfernotify(String^ reply);

  private:

	String^ getreplyTitle(String^ reply);
	String^ getFilesList(String^ reply);
	String^ getFoldersList(String^ reply);
	String^ getXMLlist(String^ reply);
	String^ getTime(String^ reply);
	String^ getFilename(String^ reply);
	String^ getPort(String^ reply);
	bool getRequireXml(String^ reply);
	
 	bool IsConnected();
    std::string toStdString(String^ pStr);
    String^ toSystemString(std::string& str);
    void setTextBlockProperties();
    void setButtonsProperties();
	void clearmeassage();
	String^ mesage;

	String^ returnMsg;
	String^ command;
	String^ localport;
	String^ dstport;
	String^ filename;
	String^ multiEnds;
	String^ ThreadNum;
	String^ requireXML;

	String^ body;
	String^ text;
	String^ path;
	String^ Pattern;
	String^ TPattern;
  };
}

#endif
