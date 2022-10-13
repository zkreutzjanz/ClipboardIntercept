#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <math.h>


#include <memory>
#include <windows.h>
using namespace std;
UINT uFormat = (UINT)(-1); 

void printBytes(unsigned char* ptr,int length,string offset){
    int printed =0;
    cout<<offset;
    while(printed<length){
        cout<<((int)(ptr+printed*sizeof(char))[0])<<"__";
        if((printed+1)%8==0)
            cout<<"\n"<<offset;
        printed++;
    }
}

bool getClipboardStandardFormatName(unsigned int uFormat,char buffer[80],int size){
    string formatName;
    
    switch(uFormat){
    case 1:
        formatName = "CF_TEXT";
        break;
    case 2:
        formatName = "CF_BITMAP";
        break;
    case 3:
        formatName = "CF_METAFILEPICT";
        break;
    case 4:
        formatName = "CF_SYLK";
        break;
    case 5:
        formatName = "CF_DIF";
        break;
    case 6:
        formatName = "CF_TIFF";
        break;
    case 7:
        formatName = "CF_OEMTEXT";
        break;
    case 8:
        formatName = "CF_DIB";
        break;
    case 9:
        formatName = "CF_PALETTE";
        break;
    case 10:
        formatName = "CF_PENDATA";
        break;
    case 11:
        formatName = "CF_RIFF";
        break;
    case 12:
        formatName = "CF_WAVE";
        break;
    case 13:
        formatName = "CF_UNICODETEXT";
        break;
    case 14:
        formatName = "CF_ENHMETAFILE";
        break;
    case 15:
        formatName = "CF_HDROP";
        break;
    case 16:
        formatName = "CF_LOCALE";
        break;
    case 17:
        formatName = "CF_DIBV5";
        break;
    default:
        return false;
    }
    int i =0;
    for(i;i<formatName.length();i++){
        buffer[i]=formatName.at(i);
    }
    for(i;i<size;i++){
        buffer[i]='\0';
    }

    return true;
}


//find a better way to do this 
unsigned int getClipboardStandardFormatValue(char* name){

    if((string)name=="CF_TEXT")             {return 1;}
    else if((string)name=="CF_BITMAP")      {return 2;}
    else if((string)name=="CF_METAFILEPICT"){return 3;}
    else if((string)name=="CF_SYLK")        {return 4;}
    else if((string)name=="CF_DIF")         {return 5;}
    else if((string)name=="CF_TIFF")        {return 6;}
    else if((string)name=="CF_OEMTEXT")     {return 7;}
    else if((string)name=="CF_DIB")         {return 8;}
    else if((string)name=="CF_PALETTE")     {return 9;}
    else if((string)name=="CF_PENDATA")     {return 10;}
    else if((string)name=="CF_RIFF")        {return 11;}
    else if((string)name=="CF_WAVE")        {return 12;}
    else if((string)name=="CF_UNICODETEXT") {return 13;}
    else if((string)name=="CF_ENHMETAFILE") {return 14;}
    else if((string)name=="CF_HDROP")       {return 15;}
    else if((string)name=="CF_LOCALE")      {return 16;}
    else if((string)name=="CF_DIBV5")       {return 17;}
    else                                    {return 0;}

}



class clipboardSnip{
    private:
        
        
        
        int nameLength;
    public:
        char * name;
        unsigned char * data;
        int dataLength;
        clipboardSnip(){};

        bool setData(unsigned char * _data,int _dataLength){
            try{
                data = new unsigned char[_dataLength];
                memcpy(data,_data,_dataLength*sizeof(unsigned char));
                dataLength = _dataLength;
            }catch(exception ex){
                return false;
            }
            return true;
        };
        bool setName(char * _name,int _nameLength){
            try{
                name = new char[_nameLength];
                memcpy(name,_name,_nameLength*sizeof(char));
                nameLength = _nameLength;
            }catch(exception ex){
                return false;
            }
            return true;
        };
        void print(string offset){
            cout<<offset<<"-"<<name<<"-:\n";
            printBytes(data,dataLength,offset);
            
        }
        void printName(string offset){
            cout<<offset<<"-"<<name<<"-\n";
        }

};


class clipboardContents{
    private:
        
    public:
        vector<clipboardSnip> snips;
        clipboardContents(){};

        bool addClipboardSnip(clipboardSnip _snip){
            try{ 
                snips.push_back(_snip);
            }catch(exception ex){
                return false;
            }
            return true;
        }

        void print(string offset){
            for(clipboardSnip snip : snips){
                cout<<"\n";
                snip.print(offset);
            }
            if(!snips.size()) cout<<offset<<"~NO DATA~\n";
        }

        void printNames(string offset){
            for(clipboardSnip snip : snips){
                snip.printName(offset);
            }
            if(!snips.size()) cout<<offset<<"~NO DATA~\n";
        }

        void clear(){
            snips.clear();
        }
};


//taken from SO somewhere\/
string GetErrorMessage(DWORD dwErrorCode)
{
    LPTSTR psz{ nullptr };
    const DWORD cchMsg = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM
                                         | FORMAT_MESSAGE_IGNORE_INSERTS
                                         | FORMAT_MESSAGE_ALLOCATE_BUFFER,
                                       NULL, // (not used with FORMAT_MESSAGE_FROM_SYSTEM)
                                       dwErrorCode,
                                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                       reinterpret_cast<LPTSTR>(&psz),
                                       0,
                                       NULL);
    if (cchMsg > 0)
    {
        // Assign buffer to smart pointer with custom deleter so that memory gets released
        // in case String's c'tor throws an exception.
        auto deleter = [](void* p) { ::LocalFree(p); };
        std::unique_ptr<TCHAR, decltype(deleter)> ptrBuffer(psz, deleter);
        string st;
        
        return string((const char *)ptrBuffer.get(), cchMsg);
    }
    else
    {
        auto error_code{ ::GetLastError() };
        throw std::system_error( error_code, std::system_category(),
                                 "Failed to retrieve error message string.");
    }
}

class clipboard{
    private:
    


    public:

    clipboard(){};

    clipboardContents getClipboardData(){
        clipboardContents _clipboardContents;

        if(!OpenClipboard(NULL)) throw runtime_error("Cannot open Clipboard");

        unsigned int currentFormat = EnumClipboardFormats(0); 

        char* currentFormatName;
        char currentFormatBuffer[80];
        while(currentFormat){
            clipboardSnip currentSnip;

            try{
                if(GetClipboardFormatNameA(currentFormat,currentFormatBuffer,sizeof(currentFormatBuffer)))
                    currentFormatName = currentFormatBuffer;
                else if(getClipboardStandardFormatName((unsigned int)currentFormat,currentFormatBuffer,sizeof(currentFormatBuffer)))
                    currentFormatName = currentFormatBuffer;
                else throw runtime_error("No formatName found from currentFormat");;

                currentSnip.setName(currentFormatName,sizeof(currentFormatBuffer));
                
                SetLastError(0);
                HANDLE h = GetClipboardData(currentFormat);
                DWORD err=GetLastError();
                if(err) throw runtime_error("GetClipboardData(currentFormat) FAIL ::"+GetErrorMessage(err));

                SetLastError(0);
                unsigned char* p = (unsigned char*)GlobalLock(h);
                err=GetLastError();
                if(err) throw runtime_error("GlobalLock FAIL ::"+GetErrorMessage(err));
                
                SetLastError(0);
                GlobalUnlock(h);
                err=GetLastError();
                if(err) throw runtime_error("GlobalUnLock FAIL ::"+GetErrorMessage(err));
                
                if(!currentSnip.setData(p,GlobalSize(h))) throw runtime_error("Cannot add this data to snip");
                    
                if(!_clipboardContents.addClipboardSnip(currentSnip)) throw runtime_error("Cannot add snip to contents");
             
                   
                
            }catch(runtime_error e){
               cout<<"Format <"<<currentFormat<<"> :: get failure :: "<<e.what();
            }catch(exception e){
                cout<<"Format <"<<currentFormat<<"> :: get failure(NOT runtime):: "<<e.what();
            }
            currentFormat = EnumClipboardFormats(currentFormat);
            
        }
        if(!CloseClipboard()) throw runtime_error("Cannot close Clipboard");
        
        return _clipboardContents;
    }

    void clearClipboardData(){
        if(!OpenClipboard(NULL)) throw runtime_error("Cannot open Clipboard");

        if(!EmptyClipboard()) throw runtime_error("Cannot empty Clipboard");

        if(!CloseClipboard()) throw runtime_error("Cannot close Clipboard");
        
    }

    void setClipBoardData(clipboardContents _clipboardContents){
        if(!OpenClipboard(NULL)) throw runtime_error("Cannot open Clipboard");

        if(!EmptyClipboard()) throw runtime_error("Cannot empty Clipboard");

        for(clipboardSnip snip : _clipboardContents.snips){
            try{
                unsigned int clipboardFormat;

                if(getClipboardStandardFormatValue(snip.name))
                    clipboardFormat = getClipboardStandardFormatValue(snip.name);
                else
                    clipboardFormat = RegisterClipboardFormatA(snip.name);
            
                HANDLE h = GlobalAlloc(GMEM_MOVEABLE,snip.dataLength);
                LPVOID p = GlobalLock(h);
                memcpy(p, snip.data, snip.dataLength);
                GlobalUnlock(h);
                if(!SetClipboardData(clipboardFormat,h)) throw runtime_error("Cannot set Clipboard data");
            

            }catch(runtime_error e){
                cout<<"setClipBoardData():: Format Name <"<<snip.name<<"> :: ERROR :: "<<e.what()<<"\n";
            }


        }
    }





};

int main(){
    clipboard cb;
    clipboardContents cc =  cb.getClipboardData();
    cb.clearClipboardData();
    cc.printNames("CC Saved Value Check:  ");
    cb.getClipboardData().printNames("CC System Clipboard Check:");

    cb.setClipBoardData(cc);

    cb.getClipboardData().printNames("CC Rebuilt Clipboad Check: ");
    
}
/**
int main(){
    BOOL result;
    result = OpenClipboard(NULL);
    if(!result){cout<<"cannot open";return 0;}

    cout<<"waza";
    uFormat = EnumClipboardFormats(0); 
    LPCSTR lpFormatName; 
    char szFormatName[80];
    while (uFormat) 
    { 

        // Call an application-defined function to get the name 
        // of the clipboard format. 
 
 
        // For registered formats, get the registered name. 
        

        cout<<uFormat<<"::::";
        if(getClipboardStandardFormatName((unsigned int)uFormat)!="")
            lpFormatName = &getClipboardStandardFormatName((unsigned int)uFormat)[0];
        else{
            if (GetClipboardFormatNameA(uFormat, szFormatName, sizeof(szFormatName))) 
            lpFormatName = szFormatName; 
            else 
            lpFormatName = "(unknown)"; 
        }
        // Note that, if the format name is larger than the
        // buffer, it is truncated. 
        
        
        
 
        cout<<"\nName:"<<lpFormatName<<"\n";

        HANDLE h = GetClipboardData(uFormat);//Get Clipboard Data
		unsigned char* p = (unsigned char*)GlobalLock(h);
		GlobalUnlock(h);
        SIZE_T hLength = GlobalSize(h);
        cout<<"  DataLength:"<<hLength<<"\n";
		printBytes(p,hLength,"__");
 
        uFormat = EnumClipboardFormats(uFormat); 
    } 

    result = CloseClipboard();
    if(!result){cout<<"cannot close";return 0;}


}
**/