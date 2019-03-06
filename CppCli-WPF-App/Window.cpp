///////////////////////////////////////////////////////////////////////////
// Window.cpp - C++\CLI implementation of WPF Application                //
//          - 687 Project #4                                             //
// ver 2.1                                                               //
// Ruizhe Cai 354520026 rcai100@syr.edu                                  //
// based on ver 2.0 by Prof. Fawcett                                     //
///////////////////////////////////////////////////////////////////////////
/*
*  To run as a Windows Application:
*  - Set Project Properties > Linker > System > Subsystem to Windows
*  - Comment out int main(...) at bottom
*  - Uncomment int _stdcall WinMain() at bottom
*  To run as a Console Application:
*  - Set Project Properties > Linker > System > Subsytem to Console
*  - Uncomment int main(...) at bottom
*  - Comment out int _stdcall WinMain() at bottom
* -Modified and function added by Ruizhe Cai 354520036
*/
#include "Window.h"
using namespace CppCliWindows;

WPFCppCliDemo::WPFCppCliDemo(String^ lclport)
{

  localport = lclport;
  // set up channel

  ObjectFactory* pObjFact = new ObjectFactory;
  pSendr_ = pObjFact->createSendr();
  pRecvr_ = pObjFact->createRecvr();
  pChann_ = pObjFact->createMockChannel(pSendr_, pRecvr_, toStdString(lclport));
  pChann_->start();
  delete pObjFact;

  // client's receive thread

  recvThread = gcnew Thread(gcnew ThreadStart(this, &WPFCppCliDemo::getMessage));
  recvThread->Start();

  // set event handlers

  this->Loaded += 
    gcnew System::Windows::RoutedEventHandler(this, &WPFCppCliDemo::OnLoaded);
  this->Closing += 
    gcnew CancelEventHandler(this, &WPFCppCliDemo::Unloading);
  hSendButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendTextBoxMessage);
  hClearButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::clear);
  hFolderBrowseButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::browseForFolder);
  hSetPatt->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addPattern);
  hTSetPatt->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addTPattern);
  hSetText->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addTextSearch);
  hSetUploadFileButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addUploadFilename);
  hSetUploadFileButton2->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addDownLoadFilename);
  hSetUploadFileButton3->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addDownLoadFilenameT);
  hSelectPathButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addPath);
  hSelectPathButton2->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::addEnds);
  hConnectButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendConnect);
  hUploadButton->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendUpload);
  hDownload->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendDownload);
  hTDownload->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendDownloadT);
  hQuery->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendQuery);
  hSearch->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::sendTextQuery);
  hSetThread->Click += gcnew RoutedEventHandler(this, &WPFCppCliDemo::setThreadNumberF);


  // set Window properties

  this->Title = "WPF C++/CLI Demo";
  this->Width = 800;
  this->Height = 700;

  // attach dock panel to Window

  this->Content = hDockPanel;
  hDockPanel->Children->Add(hStatusBar);
  hDockPanel->SetDock(hStatusBar, Dock::Bottom);
  hDockPanel->Children->Add(hGrid);

  // setup Window controls and views

  setUpTabControl();
  setUpStatusBar();
  setUpSendMessageView();
  setUpFileListView();
  setUpQueryListView();
  setUpTextSearchListView();
  setUpConnectionView();
  setUpPerformanceView();
  setUpXMLView();
}


WPFCppCliDemo::~WPFCppCliDemo()
{
  delete pChann_;
  delete pSendr_;
  delete pRecvr_;
}

bool WPFCppCliDemo::IsConnected()
{
	if (!String::IsNullOrEmpty(dstport))
		return true;
	return false;
}

void WPFCppCliDemo::setUpStatusBar()
{
  hStatusBar->Items->Add(hStatusBarItem);
  hStatus->Text = "very important messages will appear here";
  //status->FontWeight = FontWeights::Bold;
  hStatusBarItem->Content = hStatus;
  hStatusBar->Padding = Thickness(10, 2, 10, 2);
}

void WPFCppCliDemo::setUpTabControl()
{
  hGrid->Children->Add(hTabControl);
  hSendMessageTab->Header = "Send Message";
  hFileListTab->Header = "File List";
  hConnectTab->Header = "Connect";
  hQueryTab->Header = "Query & Download";
  hPerformanceTab->Header = "Performance";
  hTextSearchTab->Header = "Text Search";
  hXMLViewTab->Header = "XML Viewer";
  hTabControl->Items->Add(hSendMessageTab);
  hTabControl->Items->Add(hFileListTab);
  hTabControl->Items->Add(hQueryTab);
  hTabControl->Items->Add(hConnectTab);
  hTabControl->Items->Add(hTextSearchTab);
  hTabControl->Items->Add(hPerformanceTab);
  hTabControl->Items->Add(hXMLViewTab);
}

void WPFCppCliDemo::setUpPerformanceView()
{
	Console::Write("\n  setting up Performance view");

	hPerfGrid->Margin = Thickness(20);
	hPerformanceTab->Content = hPerfGrid;

	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hPerfGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hPerfHistory;
	hPerfHistory->Padding = Thickness(15);
	hPerfHistory->Text = "";
	hPerfHistory->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hPerfHistory->FontWeight = FontWeights::Bold;
	hPerfHistory->FontSize = 16;
	hScrollViewerPerf->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewerPerf->Content = hBorder1;
	hPerfGrid->SetRow(hScrollViewerPerf, 0);
	hPerfGrid->Children->Add(hScrollViewerPerf);
}

void WPFCppCliDemo::setUpXMLView()
{
	Console::Write("\n  setting up XML view");

	hXMLGrid->Margin = Thickness(20);
	hXMLViewTab->Content = hXMLGrid;

	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hXMLGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hXMLView;
	hXMLView->Padding = Thickness(15);
	hXMLView->Text = "";
	hXMLView->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hXMLView->FontWeight = FontWeights::Bold;
	hXMLView->FontSize = 16;
	hScrollViewerXML->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewerXML->Content = hBorder1;
	hXMLGrid->SetRow(hScrollViewerXML, 0);
	hXMLGrid->Children->Add(hScrollViewerXML);
}

void WPFCppCliDemo::setUpConnectionView()
{
	hConnectGrid->Margin = Thickness(20);
	hConnectTab->Content = hConnectGrid;

	Console::Write("\n  setting up Connection view");
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hConnectGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hConnectionHistory;
	hConnectionHistory->Padding = Thickness(15);
	hConnectionHistory->Text = "";
	hConnectionHistory->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
	hConnectionHistory->FontWeight = FontWeights::Bold;
	hConnectionHistory->FontSize = 16;
	hScrollViewerC->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
	hScrollViewerC->Content = hBorder1;
	hConnectGrid->SetRow(hScrollViewerC, 0);
	hConnectGrid->Children->Add(hScrollViewerC);

	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hConnectGrid->RowDefinitions->Add(hRow2Def);
	hConnectButton->Content = "Connect";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hConnectButton;
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 500;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hDstEndBox;
	hStackPanelC->Children->Add(hBorder2);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 10;
	hStackPanelC->Children->Add(hSpacer);
	hStackPanelC->Children->Add(hBorder4);
	hStackPanelC->Children->Add(hSpacer2);
	hStackPanelC->Orientation = Orientation::Horizontal;
	hStackPanelC->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hConnectGrid->SetRow(hStackPanelC, 1);
	hConnectGrid->Children->Add(hStackPanelC);
}

void WPFCppCliDemo::setTextBlockProperties()
{
  RowDefinition^ hRow1Def = gcnew RowDefinition();
  hSendMessageGrid->RowDefinitions->Add(hRow1Def);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hBorder1->Child = hTextBlock1;
  hTextBlock1->Padding = Thickness(15);
  hTextBlock1->Text = "";
  hTextBlock1->FontFamily = gcnew Windows::Media::FontFamily("Tahoma");
  hTextBlock1->FontWeight = FontWeights::Bold;
  hTextBlock1->FontSize = 16;
  hScrollViewer1->VerticalScrollBarVisibility = ScrollBarVisibility::Auto;
  hScrollViewer1->Content = hBorder1;
  hSendMessageGrid->SetRow(hScrollViewer1, 0);
  hSendMessageGrid->Children->Add(hScrollViewer1);
}

void WPFCppCliDemo::setButtonsProperties()
{
  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hRow2Def->Height = GridLength(75);
  hSendMessageGrid->RowDefinitions->Add(hRow2Def);
  hSendButton->Content = "Send Message";
  Border^ hBorder2 = gcnew Border();
  hBorder2->Width = 120;
  hBorder2->Height = 30;
  hBorder2->BorderThickness = Thickness(1);
  hBorder2->BorderBrush = Brushes::Black;
  hClearButton->Content = "Clear";
  hBorder2->Child = hSendButton;
  Border^ hBorder3 = gcnew Border();
  hBorder3->Width = 120;
  hBorder3->Height = 30;
  hBorder3->BorderThickness = Thickness(1);
  hBorder3->BorderBrush = Brushes::Black;
  hBorder3->Child = hClearButton;
  Border^ hBorder4 = gcnew Border();
  hBorder4->Width = 300;
  hBorder4->Height = 30;
  hBorder4->BorderThickness = Thickness(1);
  hBorder4->BorderBrush = Brushes::Black;
  hBorder4->Child = hMessageBox;
  hStackPanel1->Children->Add(hBorder2);
  TextBlock^ hSpacer = gcnew TextBlock();
  hSpacer->Width = 10;
  TextBlock^ hSpacer2 = gcnew TextBlock();
  hSpacer2->Width = 10;
  hStackPanel1->Children->Add(hSpacer);
  hStackPanel1->Children->Add(hBorder4);
  hStackPanel1->Children->Add(hSpacer2);
  hStackPanel1->Children->Add(hBorder3);
  hStackPanel1->Orientation = Orientation::Horizontal;
  hStackPanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hSendMessageGrid->SetRow(hStackPanel1, 1);
  hSendMessageGrid->Children->Add(hStackPanel1);
}

void WPFCppCliDemo::setUpSendMessageView()
{
  Console::Write("\n  setting up sendMessage view");
  hSendMessageGrid->Margin = Thickness(20);
  hSendMessageTab->Content = hSendMessageGrid;

  setTextBlockProperties();
  setButtonsProperties();
}

std::string WPFCppCliDemo::toStdString(String^ pStr)
{
  std::string dst;
  for (int i = 0; i < pStr->Length; ++i)
    dst += (char)pStr[i];
  return dst;
}

void WPFCppCliDemo::sendMessage()
{
  pSendr_->postMessage(toStdString(mesage));
  Console::Write("\n  sent message \n" + mesage );
  hStatus->Text = "Sent message";
  clearmeassage();
}

void WPFCppCliDemo::sendTextBoxMessage(Object^ obj, RoutedEventArgs^ args)
{
	command = "sendmessage";
	mesage = "command:" + command + "\nsrcaddr:localhost+" + localport + "\ndstaddr:localhost+"
		+ dstport + "\nmessage:" + hMessageBox->Text;
	sendMessage();
	hMessageBox->Clear();
	Console::Write("\n  sent message");
	hStatus->Text = "Sent message";
}

void WPFCppCliDemo::addPattern(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hPatt->Text))
	{
		Console::Write("\n Pattern " + hPatt->Text + " added\n");
		hPatterns->Text += hPatt->Text + ", ";
		Pattern += "<pattern>\"" + hPatt->Text + "\"</pattern>";
		hStatus->Text = "Pattern Added";
		hPatt->Clear();
	}
	else
		hStatus->Text = "Please enter a pattern to add";
}

void WPFCppCliDemo::addTPattern(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hTPatt->Text))
	{
		Console::Write("\n Pattern " + hTPatt->Text + " added\n");
		hTPatterns->Text += hTPatt->Text + ", ";
		TPattern += "<pattern>\"" + hTPatt->Text + "\"</pattern>";
		hStatus->Text = "Pattern Added";
		hTPatt->Clear();
	}
	else
		hStatus->Text = "Please enter a pattern to add";
}

void WPFCppCliDemo::setThreadNumberF(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hThreadNumber->Text))
	{
		Console::Write("\n ThreadNum " + hThreadNumber->Text + " specified\n");
		hTHN->Text = hThreadNumber->Text;
		hStatus->Text = "Thread# Specified";
		hThreadNumber->Clear();
	}
	else
		hStatus->Text = "Please enter a thread number to spicify";
}

void WPFCppCliDemo::addTextSearch(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hText->Text))
	{
		Console::Write("\n Text " + hText->Text + " specified\n");
		hTargetText->Text = hText->Text;
		text = "<text>\"" + hText->Text + "\"</text";
		hStatus->Text = "Text Specified";
		hText->Clear();
	}
	else
		hStatus->Text = "Please enter some text to spicify";
}

String^ WPFCppCliDemo::toSystemString(std::string& str)
{
  StringBuilder^ pStr = gcnew StringBuilder();
  for (size_t i = 0; i < str.size(); ++i)
    pStr->Append((Char)str[i]);
  return pStr->ToString();
}

void WPFCppCliDemo::addText(String^ msg)
{
  hTextBlock1->Text += msg + "\n\n";
}

void WPFCppCliDemo::sendConnect(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hDstEndBox->Text->ToString()))
	{
		command = "connect";
		dstport = hDstEndBox->Text->ToString();
		mesage = "command:" + command + "\nsrcaddr:localhost+" + localport + "\ndstaddr:localhost+" + dstport;
		sendMessage();
		hDstEndBox->Clear();
	}
	else
		hStatus->Text = "Please Enter a Port number to connect";
}

void WPFCppCliDemo::sendUpload(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hFN1CT->Text->ToString()) && IsConnected())
	{
		command = "upload";
		filename = hFN1CT->Text->ToString();
		mesage = "command:" + command + "\nsrcaddr:localhost+" + localport + "\ndstaddr:localhost+"
			+ dstport + "\nfilename:" + filename;
		sendMessage();
		hFN1CT->Text = String::Empty;
		filename = String::Empty;
	}
	else
	{
		if (String::IsNullOrEmpty(dstport))
			hStatus->Text = "Please connect to a machine first";
		else
			hStatus->Text = "Please select a file to dwonload";
	}
}

void WPFCppCliDemo::sendDownload(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hFN2CT->Text->ToString()) && hFN2CT->Text->ToString()->Contains("@:"))
	{
		command = "download";
		String^ delim = "@:";
		array<Char>^ delimer = delim->ToCharArray();
		array<String^>^ results;
		results = hFN2CT->Text->Split(delimer);
		filename = results[2];
		dstport = results[0];
		mesage = "command:" + command + "\nsrcaddr:localhost+" + localport + "\ndstaddr:localhost+"
			+ dstport +"\nHasPath:true"+ "\nfilename:" + filename;
		sendMessage();
		hFN2CT->Text = String::Empty;
		filename = String::Empty;
	}
	else
	{
			hStatus->Text = "Please select a file to download";
	}
}

void WPFCppCliDemo::sendDownloadT(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hFN3CT->Text->ToString()) && hFN3CT->Text->ToString()->Contains("@:"))
	{
		command = "download";
		String^ delim = "@:";
		array<Char>^ delimer = delim->ToCharArray();
		array<String^>^ results;
		results = hFN3CT->Text->Split(delimer);
		filename = results[2];
		dstport = results[0];
		mesage = "command:" + command + "\nsrcaddr:localhost+" + localport + "\ndstaddr:localhost+"
			+ dstport +"\nHasPath:true"+ "\nfilename:" + filename;
		sendMessage();
		hFN3CT->Text = String::Empty;
	}
	else
	{
		hStatus->Text = "Please select a file to upload";
	}
}

void WPFCppCliDemo::clearmeassage()
{
	command = String::Empty;
	mesage = String::Empty;
	body = String::Empty;
}

void WPFCppCliDemo::sendQuery(Object^ obj, RoutedEventArgs^ args)
{
	if (IsConnected())
	{
		command = "filequery";
		requireXML = "false";
		if (hifXML->IsChecked)
			requireXML = "true";
		mesage = "command:" + command + "\nsrcaddr:localhost+" + localport + "\ndstaddr:localhost+"
			+ dstport + "\nRequireXml:" + requireXML;
		if (!String::IsNullOrEmpty(hPatterns->Text->ToString()))
			body = Pattern;
		Pattern = String::Empty;
		hPatterns->Text = String::Empty;
		if (!String::IsNullOrEmpty(hPTHCT->Text->ToString()))
			body += path;
		path = String::Empty;
		hPTHCT->Text = String::Empty;
		if (!String::IsNullOrEmpty(body))
		{
			body = "<Filter>" + body + "</Filter>";
			mesage += "\nbody:" + body;
		}
		sendMessage();
	}
	else
		hStatus->Text = "Please connect to a machine first";

}

void WPFCppCliDemo::sendTextQuery(Object^ obj, RoutedEventArgs^ args)
{
	if (!String::IsNullOrEmpty(hPTHCT2->Text->ToString()))
	{
		command = "textsearch";
		requireXML = "false";
		if (hTifXML->IsChecked)
			requireXML = "true";
		ThreadNum = hTHN->Text;
		hTHN->Text = "1";
		mesage = "command:" + command + "\nsrcaddr:localhost+" + localport 
				+ "\nmultidst:" + multiEnds +"end" + "\nRequireXml:" + requireXML + "\nThreadNumber:" + ThreadNum;
		multiEnds = String::Empty;
		hPTHCT2->Text = "";
		if (!String::IsNullOrEmpty(hTPatterns->Text->ToString()))
			body = TPattern;
		TPattern = String::Empty;
		hTPatterns->Text = String::Empty;
		if (!String::IsNullOrEmpty(hTargetText->Text))
			body += text;
		text = String::Empty;
		hTargetText->Text = String::Empty;
		if (!String::IsNullOrEmpty(body))
		{
			body = "<Filter>" + body + "</Filter>";
			mesage += "\nbody:" + body;
		}
		sendMessage();
	}
	else
		hStatus->Text = "Please choose at least a machine first";

}


void WPFCppCliDemo::addUploadFilename(Object^ obj, RoutedEventArgs^ args)
{
	if (hListBox->SelectedIndex != -1)
	{
		hFN1CT->Text = hListBox->SelectedItem->ToString();
		Console::Write("\n File" + hListBox->SelectedItem->ToString() + " specified");
		hStatus->Text = "File selected";
	}
	else
		hStatus->Text = "Please Select a File";
}

void WPFCppCliDemo::addDownLoadFilename(Object^ obj, RoutedEventArgs^ args)
{
	if (hFiles->SelectedIndex != -1)
	{
		hFN2CT->Text = hFiles->SelectedItem->ToString();
		Console::Write("\n File" + hFiles->SelectedItem->ToString() + " specified");
		hStatus->Text = "File selected";
	}
	else
		hStatus->Text = "Please Select a File";
}

void WPFCppCliDemo::addDownLoadFilenameT(Object^ obj, RoutedEventArgs^ args)
{
	if (hMatchedFiles->SelectedIndex != -1)
	{
		hFN3CT->Text = hMatchedFiles->SelectedItem->ToString();
		Console::Write("\n File" + hMatchedFiles->SelectedItem->ToString() + " specified");
		hStatus->Text = "File selected";
	}
	else
		hStatus->Text = "Please Select a File";
}

void WPFCppCliDemo::addPath(Object^ obj, RoutedEventArgs^ args)
{
	if (hFolders->SelectedIndex != -1)
	{
		hPTHCT->Text = hFolders->SelectedItem->ToString();
		Console::Write("\n Path" + hFolders->SelectedItem->ToString() + " specified");
		String^ pth = hFolders->SelectedItem->ToString();
		int found = 0;
		found = pth->IndexOf("@:");
		pth = pth->Substring(found + 2);
		path = "<path>\"" + pth + "\"</path>";
		hStatus->Text = "Path selected";
	}
	else
		hStatus->Text = "Please Select a Path";
}

void WPFCppCliDemo::addEnds(Object^ obj, RoutedEventArgs^ args)
{
	int count;
	for each (CheckBox^ ob in hMachines->Items)
	{
		if (ob->IsChecked)
		{
			multiEnds += ob->Content->ToString()+"&";
			hPTHCT2->Text += ob->Content->ToString() + ";";
			count++;
		}
		else
			continue;
	}
	if (count == 0)
	{
		multiEnds = String::Empty;
		hPTHCT2->Text = String::Empty;
		hStatus->Text = "Please check some machines to add";
	}
	else
	{
		Console::Write("\nMachines\n" + multiEnds + "Selected");
		hStatus->Text = "Machines Selected";
	}
}



void WPFCppCliDemo::getMessage()
{
  // recvThread runs this function

  while (true)
  {
    std::cout << "\n  receive thread calling getMessage()";
    std::string msg = pRecvr_->getMessage();
    String^ sMsg = toSystemString(msg);
    array<String^>^ args = gcnew array<String^>(1);
    args[0] = sMsg;
	String^ title = getreplyTitle(sMsg);
	Console::Write("GuiReceived" + sMsg);
	if (title == "connect")
	{	
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::ConnectionHistory);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		continue;
	}

	if (title == "filequery")
	{
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::QueryView);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		continue;
	}

	if (title == "textsearch")
	{
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::TextSearchView);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		continue;
	}

	if (title == "upload" || title == "receivefile")
	{
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::filetransfernotify);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		continue;
	}

	if (title == "receivemessage")
	{
		Action<String^>^ act = gcnew Action<String^>(this, &WPFCppCliDemo::addText);
		Dispatcher->Invoke(act, args);  // must call addText on main UI thread
		continue;
	}
  }
}

void WPFCppCliDemo::clear(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  cleared message text");
  hStatus->Text = "Cleared message";
  hTextBlock1->Text = "";
}

void WPFCppCliDemo::setUpQueryListView()
{
  Console::Write("\n setting up QueryList view");
  hQueryListGrid->Margin = Thickness(20);
  hQueryTab->Content = hQueryListGrid;

  RowDefinition^ hRow0Def = gcnew RowDefinition();
  hRow0Def->Height = GridLength(15);
  hQueryListGrid->RowDefinitions->Add(hRow0Def);
  hDIRS->Text = "SubFolders of Root";
  Border^ hBorder0 = gcnew Border();
  hBorder0->BorderThickness = Thickness(1);
  hBorder0->BorderBrush = Brushes::Black;
  hBorder0->Child = hDIRS;
  hQueryListGrid->SetRow(hBorder0, 0);
  hQueryListGrid->Children->Add(hBorder0);

  RowDefinition^ hRow1Def = gcnew RowDefinition();
  hQueryListGrid->RowDefinitions->Add(hRow1Def);
  Border^ hBorder1 = gcnew Border();
  hBorder1->BorderThickness = Thickness(1);
  hBorder1->BorderBrush = Brushes::Black;
  hBorder1->Child = hFolders;
  hQueryListGrid->SetRow(hBorder1, 1);
  hQueryListGrid->Children->Add(hBorder1);

  RowDefinition^ hRow9Def = gcnew RowDefinition();
  hRow9Def->Height = GridLength(15);
  hQueryListGrid->RowDefinitions->Add(hRow9Def);
  hFLS->Text= "Files";
  Border^ hBorder9 = gcnew Border();
  hBorder9->BorderThickness = Thickness(1);
  hBorder9->BorderBrush = Brushes::Black;
  hBorder9->Child = hFLS;
  hQueryListGrid->SetRow(hBorder9, 2);
  hQueryListGrid->Children->Add(hBorder9);

  RowDefinition^ hRow2Def = gcnew RowDefinition();
  hQueryListGrid->RowDefinitions->Add(hRow2Def);
  Border^ hBorder2 = gcnew Border();
  hBorder2->BorderThickness = Thickness(1);
  hBorder2->BorderBrush = Brushes::Black;
  hBorder2->Child = hFiles;
  hQueryListGrid->SetRow(hBorder2, 3);
  hQueryListGrid->Children->Add(hBorder2);

  RowDefinition^ hRow3Def = gcnew RowDefinition();
  hRow3Def->Height = GridLength(40);
  hQueryListGrid->RowDefinitions->Add(hRow3Def);
  hSetPatt->Content = "Add Pattern";
  Border^ hBorder3 = gcnew Border();
  hBorder3->Width = 120;
  hBorder3->Height = 30;
  hBorder3->BorderThickness = Thickness(1);
  hBorder3->BorderBrush = Brushes::Black;
  hBorder3->Child = hSetPatt;

  Border^ hBorder6 = gcnew Border();
  hBorder6->Width = 300;
  hBorder6->Height = 30;
  hBorder6->BorderThickness = Thickness(1);
  hBorder6->BorderBrush = Brushes::Black;
  hBorder6->Child = hPatt;


  Border^ hBorderIX = gcnew Border();
  hifXML->Content = "Retrun XML";
  hBorderIX->Width = 90;
  hBorderIX->Height = 18;
  hBorderIX->BorderThickness = Thickness(1);
  hBorderIX->BorderBrush = Brushes::Black;
  hBorderIX->Child = hifXML;

  hSetUploadFileButton2->Content = "Select File";
  Border^ hBorderFB = gcnew Border();
  hBorderFB->Width = 120;
  hBorderFB->Height = 30;
  hBorderFB->BorderThickness = Thickness(1);
  hBorderFB->BorderBrush = Brushes::Black;
  hBorderFB->Child = hSetUploadFileButton2;

  hSelectPathButton->Content = "Select Path";
  Border^ hBorderSPB = gcnew Border();
  hBorderSPB->Width = 120;
  hBorderSPB->Height = 30;
  hBorderSPB->BorderThickness = Thickness(1);
  hBorderSPB->BorderBrush = Brushes::Black;
  hBorderSPB->Child = hSelectPathButton;


  RowDefinition^ hRow4Def = gcnew RowDefinition();
  hRow4Def->Height = GridLength(40);
  hQueryListGrid->RowDefinitions->Add(hRow4Def);
  hQuery->Content = "Query";
  Border^ hBorder4 = gcnew Border();
  hBorder4->Width = 120;
  hBorder4->Height = 30;
  hBorder4->BorderThickness = Thickness(1);
  hBorder4->BorderBrush = Brushes::Black;
  hBorder4->Child = hQuery;

  hDownload->Content = "Download";
  Border^ hBorder5 = gcnew Border();
  hBorder5->Width = 120;
  hBorder5->Height = 30;
  hBorder5->BorderThickness = Thickness(1);
  hBorder5->BorderBrush = Brushes::Black;
  hBorder5->Child = hDownload;

  hPatternPanel->Children->Add(hBorder3);
  TextBlock^ hSpacer = gcnew TextBlock();
  hSpacer->Width = 10;
  hPatternPanel->Children->Add(hSpacer);
  hPatternPanel->Children->Add(hBorder6);
  TextBlock^ hSpacerIX = gcnew TextBlock();
  hSpacerIX->Width = 10;
  hPatternPanel->Children->Add(hSpacerIX);
  hPatternPanel->Children->Add(hBorderIX);
  hPatternPanel->Orientation = Orientation::Horizontal;
  hPatternPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hQueryListGrid->SetRow(hPatternPanel, 4);
  hQueryListGrid->Children->Add(hPatternPanel);

  RowDefinition^ hRowPDef = gcnew RowDefinition();
  hRowPDef->Height = GridLength(20);
  hQueryListGrid->RowDefinitions->Add(hRowPDef);
  Border^ hBorderP = gcnew Border();
  hBorderP->Width = 600;
  hBorderP->Height = 20;
  hBorderP->BorderThickness = Thickness(1);
  hBorderP->BorderBrush = Brushes::Black;
  hBorderP->Child = hPatterns;
  Border^ hBorderPT = gcnew Border();
  hBorderPT->Width = 90;
  hBorderPT->Height = 20;
  hBorderPT->BorderThickness = Thickness(1);
  hBorderPT->BorderBrush = Brushes::Black;
  hPT->Text = "Added Patterns : ";
  hBorderPT->Child = hPT;
  hAddedPatterns->Children->Add(hBorderPT);
  hAddedPatterns->Children->Add(hBorderP);
  hAddedPatterns->Orientation = Orientation::Horizontal;
  hAddedPatterns->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hQueryListGrid->SetRow(hAddedPatterns, 6);
  hQueryListGrid->Children->Add(hAddedPatterns);

  hButtonPanel->Children->Add(hBorder4);
  TextBlock^ hSpacer1 = gcnew TextBlock();
  hSpacer1->Width = 10;
  hButtonPanel->Children->Add(hSpacer1);
  hButtonPanel->Children->Add(hBorder5);
  TextBlock^ hSpacerFB = gcnew TextBlock();
  hSpacerFB->Width = 10;
  hButtonPanel->Children->Add(hSpacerFB);
  hButtonPanel->Children->Add(hBorderFB);
  TextBlock^ hSpacerSPTB = gcnew TextBlock();
  hSpacerSPTB->Width = 10;
  hButtonPanel->Children->Add(hSpacerSPTB);
  hButtonPanel->Children->Add(hBorderSPB);
  hButtonPanel->Orientation = Orientation::Horizontal;
  hButtonPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hQueryListGrid->SetRow(hButtonPanel, 5);
  hQueryListGrid->Children->Add(hButtonPanel);

  RowDefinition^ hRowPTHDef = gcnew RowDefinition();
  hRowPTHDef->Height = GridLength(20);
  hQueryListGrid->RowDefinitions->Add(hRowPTHDef);
  Border^ hBorderPTH = gcnew Border();
  hBorderPTH->Width = 600;
  hBorderPTH->Height = 20;
  hBorderPTH->BorderThickness = Thickness(1);
  hBorderPTH->BorderBrush = Brushes::Black;
  hBorderPTH->Child = hPTHCT;
  Border^ hBorderPTHT = gcnew Border();
  hBorderPTHT->Width = 90;
  hBorderPTHT->Height = 20;
  hBorderPTHT->BorderThickness = Thickness(1);
  hBorderPTHT->BorderBrush = Brushes::Black;
  hPTH->Text = "Selected Path : ";
  hBorderPTHT->Child = hPTH;
  hSelectPathPanel->Children->Add(hBorderPTHT);
  hSelectPathPanel->Children->Add(hBorderPTH);
  hSelectPathPanel->Orientation = Orientation::Horizontal;
  hSelectPathPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hQueryListGrid->SetRow(hSelectPathPanel, 7);
  hQueryListGrid->Children->Add(hSelectPathPanel);

  RowDefinition^ hRowFFDef = gcnew RowDefinition();
  hRowFFDef->Height = GridLength(20);
  hQueryListGrid->RowDefinitions->Add(hRowFFDef);
  Border^ hBorderFF = gcnew Border();
  hBorderFF->Width = 600;
  hBorderFF->Height = 20;
  hBorderFF->BorderThickness = Thickness(1);
  hBorderFF->BorderBrush = Brushes::Black;
  hBorderFF->Child = hFN2CT;
  Border^ hBorderFFT = gcnew Border();
  hBorderFFT->Width = 90;
  hBorderFFT->Height = 20;
  hBorderFFT->BorderThickness = Thickness(1);
  hBorderFFT->BorderBrush = Brushes::Black;
  hFN2->Text = "Selected File : ";
  hBorderFFT->Child = hFN2;
  hFILEpanel2->Children->Add(hBorderFFT);
  hFILEpanel2->Children->Add(hBorderFF);
  hFILEpanel2->Orientation = Orientation::Horizontal;
  hFILEpanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
  hQueryListGrid->SetRow(hFILEpanel2, 8);
  hQueryListGrid->Children->Add(hFILEpanel2);

}

void WPFCppCliDemo::setUpTextSearchListView()
{
	Console::Write("\n setting up TextSearchList view");
	hTestSearchGrid->Margin = Thickness(20);
	hTextSearchTab->Content = hTestSearchGrid;

	RowDefinition^ hRow0Def = gcnew RowDefinition();
	hRow0Def->Height = GridLength(15);
	hTestSearchGrid->RowDefinitions->Add(hRow0Def);
	hFlable->Text = "Files";
	Border^ hBorder0 = gcnew Border();
	hBorder0->BorderThickness = Thickness(1);
	hBorder0->BorderBrush = Brushes::Black;
	hBorder0->Child = hFlable;
	hTestSearchGrid->SetRow(hBorder0, 0);
	hTestSearchGrid->Children->Add(hBorder0);

	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hTestSearchGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hMatchedFiles;
	hTestSearchGrid->SetRow(hBorder1, 1);
	hTestSearchGrid->Children->Add(hBorder1);

	RowDefinition^ hRow9Def = gcnew RowDefinition();
	hRow9Def->Height = GridLength(15);
	hTestSearchGrid->RowDefinitions->Add(hRow9Def);
	hMlable->Text = "Machines";
	Border^ hBorder9 = gcnew Border();
	hBorder9->BorderThickness = Thickness(1);
	hBorder9->BorderBrush = Brushes::Black;
	hBorder9->Child = hMlable;
	hTestSearchGrid->SetRow(hBorder9, 2);
	hTestSearchGrid->Children->Add(hBorder9);

	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hTestSearchGrid->RowDefinitions->Add(hRow2Def);
	Border^ hBorder2 = gcnew Border();
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hMachines;
	hTestSearchGrid->SetRow(hBorder2, 3);
	hTestSearchGrid->Children->Add(hBorder2);

	RowDefinition^ hRow3Def = gcnew RowDefinition();
	hRow3Def->Height = GridLength(40);
	hTestSearchGrid->RowDefinitions->Add(hRow3Def);
	hSetText->Content = "Set Text";
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hSetText;

	Border^ hBorder6 = gcnew Border();
	hBorder6->Width = 300;
	hBorder6->Height = 30;
	hBorder6->BorderThickness = Thickness(1);
	hBorder6->BorderBrush = Brushes::Black;
	hBorder6->Child = hText;

	Border^ hBorderIX = gcnew Border();
	hBorderIX->Width = 90;
	hBorderIX->Height = 18;
	hTifXML->Content = "Return XML";
	hBorderIX->BorderThickness = Thickness(1);
	hBorderIX->BorderBrush = Brushes::Black;
	hBorderIX->Child = hTifXML;


	hSetUploadFileButton3->Content = "Select File";
	Border^ hBorderFB = gcnew Border();
	hBorderFB->Width = 120;
	hBorderFB->Height = 30;
	hBorderFB->BorderThickness = Thickness(1);
	hBorderFB->BorderBrush = Brushes::Black;
	hBorderFB->Child = hSetUploadFileButton3;


	hSelectPathButton2->Content = "Select Machines";
	Border^ hBorderSPB = gcnew Border();
	hBorderSPB->Width = 120;
	hBorderSPB->Height = 30;
	hBorderSPB->BorderThickness = Thickness(1);
	hBorderSPB->BorderBrush = Brushes::Black;
	hBorderSPB->Child = hSelectPathButton2;


	hSetThread->Content = "Set Thread Number";
	Border^ hBorder7 = gcnew Border();
	hBorder7->Width = 120;
	hBorder7->Height = 30;
	hBorder7->BorderThickness = Thickness(1);
	hBorder7->BorderBrush = Brushes::Black;
	hBorder7->Child = hSetThread;

	Border^ hBorder8 = gcnew Border();
	hBorder8->Width = 120;
	hBorder8->Height = 30;
	hBorder8->BorderThickness = Thickness(1);
	hBorder8->BorderBrush = Brushes::Black;
	hBorder8->Child = hThreadNumber;


	RowDefinition^ hRow4Def = gcnew RowDefinition();
	hRow4Def->Height = GridLength(40);
	hTestSearchGrid->RowDefinitions->Add(hRow4Def);
	hSearch->Content = "Search";
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 120;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hSearch;

	hTDownload->Content = "Download";
	Border^ hBorder5 = gcnew Border();
	hBorder5->Width = 120;
	hBorder5->Height = 30;
	hBorder5->BorderThickness = Thickness(1);
	hBorder5->BorderBrush = Brushes::Black;
	hBorder5->Child = hTDownload;

	Border^ hBorderTHN = gcnew Border();
	hTHN->Text = "1";
	hTHN->FontSize=16;
	hBorderTHN->Width = 30;
	hBorderTHN->Height = 30;
	hBorderTHN->BorderThickness = Thickness(1);
	hBorderTHN->BorderBrush = Brushes::Black;
	hBorderTHN->Child = hTHN;

	hTextPanel->Children->Add(hBorderSPB);
	TextBlock^ hSpacerSPTB = gcnew TextBlock();
	hSpacerSPTB->Width = 10;
	hTextPanel->Children->Add(hSpacerSPTB);
	hTextPanel->Children->Add(hBorder3);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	hTextPanel->Children->Add(hSpacer);
	hTextPanel->Children->Add(hBorder6);
	TextBlock^ hSpacerIX = gcnew TextBlock();
	hSpacerIX->Width = 10;
	hTextPanel->Children->Add(hSpacerIX);
	hTextPanel->Children->Add(hBorderIX);
	hTextPanel->Orientation = Orientation::Horizontal;
	hTextPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hTestSearchGrid->SetRow(hTextPanel, 4);
	hTestSearchGrid->Children->Add(hTextPanel);

	hTButtonPanel->Children->Add(hBorderFB);
	TextBlock^ hSpacerFB = gcnew TextBlock();
	hSpacerFB->Width = 10;
	hTButtonPanel->Children->Add(hSpacerFB);
	hTButtonPanel->Children->Add(hBorder4);
	TextBlock^ hSpacer1 = gcnew TextBlock();
	hSpacer1->Width = 10;
	hTButtonPanel->Children->Add(hSpacer1);
	hTButtonPanel->Children->Add(hBorder5);
	TextBlock^ hSpacerT = gcnew TextBlock();
	hSpacerT->Width = 10;
	hTButtonPanel->Children->Add(hSpacerT);
	hTButtonPanel->Children->Add(hBorder7);
	TextBlock^ hSpacerR = gcnew TextBlock();
	hSpacerR->Width = 10;
	hTButtonPanel->Children->Add(hSpacerR);
	hTButtonPanel->Children->Add(hBorder8);
	TextBlock^ hSpacerTHN = gcnew TextBlock();
	hSpacerTHN->Width = 10;
	hTButtonPanel->Children->Add(hSpacerTHN);
	hTButtonPanel->Children->Add(hBorderTHN);
	hTButtonPanel->Orientation = Orientation::Horizontal;
	hTButtonPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hTestSearchGrid->SetRow(hTButtonPanel, 5);
	hTestSearchGrid->Children->Add(hTButtonPanel);

	RowDefinition^ hRowADDPDef = gcnew RowDefinition();
	hRowADDPDef->Height = GridLength(40);
	hTestSearchGrid->RowDefinitions->Add(hRowADDPDef);
	hTSetPatt->Content = "Add Pattern";
	Border^ hBorderADDP = gcnew Border();
	hBorderADDP->Width = 120;
	hBorderADDP->Height = 30;
	hBorderADDP->BorderThickness = Thickness(1);
	hBorderADDP->BorderBrush = Brushes::Black;
	hBorderADDP->Child = hTSetPatt;

	Border^ hBorderADDP2 = gcnew Border();
	hBorderADDP2->Width = 300;
	hBorderADDP2->Height = 30;
	hBorderADDP2->BorderThickness = Thickness(1);
	hBorderADDP2->BorderBrush = Brushes::Black;
	hBorderADDP2->Child = hTPatt;

	hTPatternPanel->Children->Add(hBorderADDP);
	TextBlock^ hSpacerADDP = gcnew TextBlock();
	hSpacerADDP->Width = 10;
	hTPatternPanel->Children->Add(hSpacerADDP);
	hTPatternPanel->Children->Add(hBorderADDP2);
	hTPatternPanel->Orientation = Orientation::Horizontal;
	hTPatternPanel->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hTestSearchGrid->SetRow(hTPatternPanel, 6);
	hTestSearchGrid->Children->Add(hTPatternPanel);

	RowDefinition^ hRowPDef = gcnew RowDefinition();
	hRowPDef->Height = GridLength(20);
	hTestSearchGrid->RowDefinitions->Add(hRowPDef);
	Border^ hBorderP = gcnew Border();
	hBorderP->Width = 600;
	hBorderP->Height = 20;
	hBorderP->BorderThickness = Thickness(1);
	hBorderP->BorderBrush = Brushes::Black;
	hBorderP->Child = hTargetText;
	Border^ hBorderPT = gcnew Border();
	hBorderPT->Width = 90;
	hBorderPT->Height = 20;
	hBorderPT->BorderThickness = Thickness(1);
	hBorderPT->BorderBrush = Brushes::Black;
	hTT->Text = "Added Text : ";
	hBorderPT->Child = hTT;
	hAddedTexts->Children->Add(hBorderPT);
	hAddedTexts->Children->Add(hBorderP);
	hAddedTexts->Orientation = Orientation::Horizontal;
	hAddedTexts->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hTestSearchGrid->SetRow(hAddedTexts, 7);
	hTestSearchGrid->Children->Add(hAddedTexts);

	RowDefinition^ hRowPTHDef = gcnew RowDefinition();
	hRowPTHDef->Height = GridLength(20);
	hTestSearchGrid->RowDefinitions->Add(hRowPTHDef);
	Border^ hBorderPTH = gcnew Border();
	hBorderPTH->Width = 600;
	hBorderPTH->Height = 20;
	hBorderPTH->BorderThickness = Thickness(1);
	hBorderPTH->BorderBrush = Brushes::Black;
	hBorderPTH->Child = hPTHCT2;
	Border^ hBorderPTHT = gcnew Border();
	hBorderPTHT->Width = 90;
	hBorderPTHT->Height = 20;
	hBorderPTHT->BorderThickness = Thickness(1);
	hBorderPTHT->BorderBrush = Brushes::Black;
	hPTH2->Text = "Selected Machines : ";
	hBorderPTHT->Child = hPTH2;
	hSelectPathPanel2->Children->Add(hBorderPTHT);
	hSelectPathPanel2->Children->Add(hBorderPTH);
	hSelectPathPanel2->Orientation = Orientation::Horizontal;
	hSelectPathPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hTestSearchGrid->SetRow(hSelectPathPanel2, 8);
	hTestSearchGrid->Children->Add(hSelectPathPanel2);

	RowDefinition^ hRowFFDef = gcnew RowDefinition();
	hRowFFDef->Height = GridLength(20);
	hTestSearchGrid->RowDefinitions->Add(hRowFFDef);
	Border^ hBorderFF = gcnew Border();
	hBorderFF->Width = 600;
	hBorderFF->Height = 20;
	hBorderFF->BorderThickness = Thickness(1);
	hBorderFF->BorderBrush = Brushes::Black;
	hBorderFF->Child = hFN3CT;
	Border^ hBorderFFT = gcnew Border();
	hBorderFFT->Width = 90;
	hBorderFFT->Height = 20;
	hBorderFFT->BorderThickness = Thickness(1);
	hBorderFFT->BorderBrush = Brushes::Black;
	hFN3->Text = "Selected File : ";
	hBorderFFT->Child = hFN3;
	hFILEpanel3->Children->Add(hBorderFFT);
	hFILEpanel3->Children->Add(hBorderFF);
	hFILEpanel3->Orientation = Orientation::Horizontal;
	hFILEpanel3->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hTestSearchGrid->SetRow(hFILEpanel3, 9);
	hTestSearchGrid->Children->Add(hFILEpanel3);

	RowDefinition^ hRowPPATDef = gcnew RowDefinition();
	hRowPPATDef->Height = GridLength(20);
	hTestSearchGrid->RowDefinitions->Add(hRowPPATDef);
	Border^ hBorderPPAT1 = gcnew Border();
	hBorderPPAT1->Width = 600;
	hBorderPPAT1->Height = 20;
	hBorderPPAT1->BorderThickness = Thickness(1);
	hBorderPPAT1->BorderBrush = Brushes::Black;
	hBorderPPAT1->Child = hTPatterns;
	Border^ hBorderPTPAT2 = gcnew Border();
	hBorderPTPAT2->Width = 90;
	hBorderPTPAT2->Height = 20;
	hBorderPTPAT2->BorderThickness = Thickness(1);
	hBorderPTPAT2->BorderBrush = Brushes::Black;
	hTPT->Text = "Added Patterns : ";
	hBorderPTPAT2->Child = hTPT;
	hTAddedPatterns->Children->Add(hBorderPTPAT2);
	hTAddedPatterns->Children->Add(hBorderPPAT1);
	hTAddedPatterns->Orientation = Orientation::Horizontal;
	hTAddedPatterns->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hTestSearchGrid->SetRow(hTAddedPatterns, 10);
	hTestSearchGrid->Children->Add(hTAddedPatterns);

}

void WPFCppCliDemo::browseForFolder(Object^ sender, RoutedEventArgs^ args)
{
  std::cout << "\n  Browsing for folder";
  hListBox->Items->Clear();
  System::Windows::Forms::DialogResult result;
  result = hFolderBrowserDialog->ShowDialog();
  if (result == System::Windows::Forms::DialogResult::OK)
  {
    String^ path = hFolderBrowserDialog->SelectedPath;
    std::cout << "\n  opening folder \"" << toStdString(path) << "\"";
    array<String^>^ files = System::IO::Directory::GetFiles(path, L"*.*");
    for (int i = 0; i < files->Length; ++i)
      hListBox->Items->Add(files[i]);
    array<String^>^ dirs = System::IO::Directory::GetDirectories(path);
    for (int i = 0; i < dirs->Length; ++i)
      hListBox->Items->Add(L"<> " + dirs[i]);
  }
}

void WPFCppCliDemo::setUpFileListView()
{
	Console::Write("\n  setting up FileList view");
	hFileListGrid->Margin = Thickness(20);
	hFileListTab->Content = hFileListGrid;
	RowDefinition^ hRow1Def = gcnew RowDefinition();
	hFileListGrid->RowDefinitions->Add(hRow1Def);
	Border^ hBorder1 = gcnew Border();
	hBorder1->BorderThickness = Thickness(1);
	hBorder1->BorderBrush = Brushes::Black;
	hBorder1->Child = hListBox;
	hFileListGrid->SetRow(hBorder1, 0);
	hFileListGrid->Children->Add(hBorder1);

	RowDefinition^ hRow2Def = gcnew RowDefinition();
	hRow2Def->Height = GridLength(75);
	hFileListGrid->RowDefinitions->Add(hRow2Def);
	hFolderBrowseButton->Content = "Select Directory";
	Border^ hBorder2 = gcnew Border();
	hBorder2->Width = 120;
	hBorder2->Height = 30;
	hBorder2->BorderThickness = Thickness(1);
	hBorder2->BorderBrush = Brushes::Black;
	hBorder2->Child = hFolderBrowseButton;

	hUploadButton->Content = "Upload";
	Border^ hBorder3 = gcnew Border();
	hBorder3->Width = 120;
	hBorder3->Height = 30;
	hBorder3->BorderThickness = Thickness(1);
	hBorder3->BorderBrush = Brushes::Black;
	hBorder3->Child = hUploadButton;

	hSetUploadFileButton->Content = "Select File";
	Border^ hBorder4 = gcnew Border();
	hBorder4->Width = 120;
	hBorder4->Height = 30;
	hBorder4->BorderThickness = Thickness(1);
	hBorder4->BorderBrush = Brushes::Black;
	hBorder4->Child = hSetUploadFileButton;

	hStackPanel2->Children->Add(hBorder2);
	TextBlock^ hSpacer = gcnew TextBlock();
	hSpacer->Width = 10;
	hStackPanel2->Children->Add(hSpacer);
	hStackPanel2->Children->Add(hBorder3);
	TextBlock^ hSpacer2 = gcnew TextBlock();
	hSpacer2->Width = 10;
	hStackPanel2->Children->Add(hSpacer2);
	hStackPanel2->Children->Add(hBorder4);
	hStackPanel2->Orientation = Orientation::Horizontal;
	hStackPanel2->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hFileListGrid->SetRow(hStackPanel2, 1);
	hFileListGrid->Children->Add(hStackPanel2);

	RowDefinition^ hRowPDef = gcnew RowDefinition();
	hRowPDef->Height = GridLength(20);
	hFileListGrid->RowDefinitions->Add(hRowPDef);
	Border^ hBorderP = gcnew Border();
	hBorderP->Width = 600;
	hBorderP->Height = 20;
	hBorderP->BorderThickness = Thickness(1);
	hBorderP->BorderBrush = Brushes::Black;
	hBorderP->Child = hFN1CT;
	Border^ hBorderPT = gcnew Border();
	hBorderPT->Width = 90;
	hBorderPT->Height = 20;
	hBorderPT->BorderThickness = Thickness(1);
	hBorderPT->BorderBrush = Brushes::Black;
	hFN1->Text = "Selected File : ";
	hBorderPT->Child = hFN1;
	hFILEpanel1->Children->Add(hBorderPT);
	hFILEpanel1->Children->Add(hBorderP);
	hFILEpanel1->Orientation = Orientation::Horizontal;
	hFILEpanel1->HorizontalAlignment = System::Windows::HorizontalAlignment::Center;
	hFileListGrid->SetRow(hFILEpanel1, 2);
	hFileListGrid->Children->Add(hFILEpanel1);

	hFolderBrowserDialog->ShowNewFolderButton = false;
	hFolderBrowserDialog->SelectedPath = System::IO::Directory::GetCurrentDirectory();
}


void WPFCppCliDemo::OnLoaded(Object^ sender, RoutedEventArgs^ args)
{
  Console::Write("\n  Window loaded");
}
void WPFCppCliDemo::Unloading(Object^ sender, System::ComponentModel::CancelEventArgs^ args)
{
  Console::Write("\n  Window closing");
}

void WPFCppCliDemo::QueryView(String^ reply)
{
	String^ delim = "\n";
	String^ port = getPort(reply);
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	String^ fl = getFilesList(reply);
	if (!String::IsNullOrEmpty(fl))
	{
		results = fl->Split(delimer);
		hFiles->Items->Clear();
		for (int i = 0; i < results->Length; ++i)
			hFiles->Items->Add(port + "@:" + results[i]);
	}
	else
	{
		hFiles->Items->Clear();
		hFiles->Items->Add("No Result Found");
	}
	fl = getFoldersList(reply);
	if (!String::IsNullOrEmpty(fl))
	{
		results = fl->Split(delimer);
		hFolders->Items->Clear();
		for (int i = 0; i < results->Length; ++i)
			hFolders->Items->Add(port + "@:" + results[i]);
	}
	else
	{
		hFolders->Items->Clear();
		hFolders->Items->Add("No Result Found");
	}
	if (getRequireXml(reply))
		XmlViewer(reply);
	addText("\n FileQuery from " + port + " received\n");
}

void WPFCppCliDemo::filetransfernotify(String^ reply)
{
	String^ port = getPort(reply);
	String^ title = getreplyTitle(reply);
	String^ filename = getFilename(reply);
	if (title == "upload")
		addText("\n" + filename + " sucessfully downloaded  form " + port + "\n");
	if (title == "receivefile")
		addText("\n" + filename + " sucessfully received  by " + port + "\n");
}


void WPFCppCliDemo::TextSearchView(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	String^ fl = getFilesList(reply);
	if (!String::IsNullOrEmpty(fl))
	{
		results = fl->Split(delimer);
		hMatchedFiles->Items->Clear();
		for (int i = 0; i < results->Length; ++i)
			hMatchedFiles->Items->Add(results[i]);
		if (getRequireXml(reply))
			XmlViewer(reply);
	}
	else
	{
		hMatchedFiles->Items->Clear();
		hMatchedFiles->Items->Add("No Result Found");
	}
	addText("\nText search results received\n");
}


String^ WPFCppCliDemo::getreplyTitle(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	results = reply->Split(delimer);
	array<String^>^ title = gcnew array<String^>(1);
	delim = ":";
	delimer = delim->ToCharArray();
	results = results[0]->Split(delimer);
	title[0] = results[1];
	return title[0];
}

String^ WPFCppCliDemo::getFilename(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	results = reply->Split(delimer);
	String^ temp;
	int i = 0;
	for (i = 0; i < results->Length; ++i)
	{
		if (results[i]->Contains("filename:"))
		{
			temp = results[i];
			break;
		}
	}
	array<String^>^ title = gcnew array<String^>(1);
	if (i == results->Length)
		return title[0];
	delim = "filename:";
	int found = temp->IndexOf(delim);
	title[0] = temp->Substring(found+9);
	return title[0];
}

bool WPFCppCliDemo::getRequireXml(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	results = reply->Split(delimer);
	String^ temp;
	int i = 0;
	for (i = 0; i < results->Length; ++i)
	{
		if (results[i]->Contains("RequireXml:"))
		{
			temp = results[i];
			break;
		}
	}
	if (i == results->Length)
		return false;
	String^ title;
	delim = "RequireXml:";
	int found = temp->IndexOf(delim);
	title = temp->Substring(found+11);
	if (title == "true")
		return true;
	else
		return false;
}

void WPFCppCliDemo::PerformanceView(String^ reply)
{
	hPerfHistory->Text += getTime(reply) + "\n";
}

void WPFCppCliDemo::XmlViewer(String^ reply)
{
	hXMLView->Text = getXMLlist(reply);
}

void WPFCppCliDemo::ConnectionHistory(String^ reply)
{
	String^ prt = getPort(reply);
	bool exist = false;
	hConnectionHistory->Text += "Connected to " + prt + "\n";
	for each(CheckBox^ cb in hMachines->Items)
	{
		if (cb->Content->ToString() == prt)
		{
			exist = true;
			break;
		}
	}
	if (!exist)
	{
		CheckBox^ ncb = gcnew CheckBox();
		ncb->Content = prt;
		hMachines->Items->Add(ncb);
	}

	addText("\n Connected to " + prt + "\n");
}

String^ WPFCppCliDemo::getFilesList(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	results = reply->Split(delimer);
	int i = 0;
	int start = 0;
	int end = 0;
	for (i = 0; i < results->Length; ++i)
	{
		if (results[i]->Contains("filelist:"))
		{
			start = i;
		}

		if (results[i]->Contains("filelistend:here"))
		{
			end  = i -2;
			break;
		}
	}
	array<String^>^ title = gcnew array<String^>(1);
	title[0] = String::Empty;
	if (i == results->Length)
		return title[0];
	if (end < start)
		return title[0];
	delim = "filelist:";
	int found = results[start]->IndexOf(delim);
	title[0] = results[start]->Substring(found+9);
	for ( i = start+1; i < end; ++i)
		title[0] += "\n" + results[i];
	return title[0];
}

String^ WPFCppCliDemo::getFoldersList(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	results = reply->Split(delimer);
	int i = 0;
	int start = 0;
	int end = 0;
	for (i = 0; i < results->Length; ++i)
	{
		if (results[i]->Contains("folderlist:"))
		{
			start = i;
		}

		if (results[i]->Contains("folderlistend:here"))
		{
			end = i - 2;
			break;
		}
	}
	array<String^>^ title = gcnew array<String^>(1);
	title[0] = String::Empty;
	if (i == results->Length)
		return title[0];
	if (end < start)
		return title[0];
	delim = "folderlist:";
	int found = results[start]->IndexOf(delim);
	title[0] = results[start]->Substring(found+12);
	for (int i = start+1 ; i < end; ++i)
		title[0]+= "\n" + results[i];
	return title[0];
}

String^ WPFCppCliDemo::getXMLlist(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	results = reply->Split(delimer);
	int i = 0;
	int start = 0;
	int end = 0;
	for (i = 0; i < results->Length; ++i)
	{
		if (results[i]->Contains("XmlList:"))
		{
			start = i;
		}

		if (results[i]->Contains("XmlListend:here"))
		{
			end = i - 2;
			break;
		}
	}
	array<String^>^ title = gcnew array<String^>(1);
	title[0] = String::Empty;
	if (i == results->Length)
		return title[0];
	if (end < start)
		return title[0];
	delim = "XmlList:";
	int found = results[start]->IndexOf(delim);
	title[0] = results[start]->Substring(found+8);
	for (int i = start+1; i < end; ++i)
		title[0] += "\n" + results[i];
	return title[0];
}

String^ WPFCppCliDemo::getTime(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	results = reply->Split(delimer);
	String^ temp;
	for (int i = 0; i < results->Length; ++i)
	{
		if (results[i]->Contains("time:"))
		{
			temp = results[i];
			break;
		}
	}
	array<String^>^ title = gcnew array<String^>(1);
	delim = ":";
	delimer = delim->ToCharArray();
	results = temp->Split(delimer);
	title[0] = results[1];
	return title[0];
}

String^ WPFCppCliDemo::getPort(String^ reply)
{
	String^ delim = "\n";
	array<Char>^ delimer = delim->ToCharArray();
	array<String^>^ results;
	results = reply->Split(delimer);
	String^ temp;
	for (int i = 0; i < results->Length; ++i)
	{
		if (results[i]->Contains("srcaddr:"))
		{
			temp = results[i];
			break;
		}
	}
	array<String^>^ title = gcnew array<String^>(1);
	delim = "+";
	delimer = delim->ToCharArray();
	results = temp->Split(delimer);
	title[0] = results[1];
	return title[0];
}



[STAThread]
//int _stdcall WinMain()
int main(array<System::String^>^ args)
{
  Console::WriteLine(L"\n Starting WPFCppCliDemo");
  String^ port = "9050";

  Application^ app = gcnew Application();
  app->Run(gcnew WPFCppCliDemo(port));
  Console::WriteLine(L"\n\n");
}