#include "DNS.h"
#include<conio.h>

void printMenu(){
    printf("DNS Server Simulator\n");
    printf("====================\n");
    printf("|1. Resolve Name   |\n");
    printf("|2. Register IP    |\n");
    printf("|3. Print All      |\n");
    printf("|4. Exit           |\n");
    printf("====================\n");
}

void anyToContinue(){
    cout << "Enter any to continue.." << endl;
    getch();
}

int main()
{
    // server declaration example
    shared_ptr <RootServer> rootserv1 = make_shared<RootServer>();
    /* 
    * shared_ptr <TLDServer> dotcom1 = make_shared<TLDServer>();
    * shared_ptr <NameServer> ns1 = make_shared<NameServer>();
    */

    // make DNS
    DNS *myDns = new DNS(rootserv1);
    ipaddress result;

    // example manual DNS server binding
    /* 
    *    rootserv1->bindTLDServer("com",dotcom1);
    *    dotcom1->bindNameServer("binus",ns1);
    *    ns1->bindIP("*",{192,168,0,1}); // * is for non-subdomain
    *    ns1->bindIP("www",{192,168,0,2});
    */

    // to register non-subdomain use "."
    // register new IP address to DNS server
    // IP 0.0.0.0 means not found / empty
    DomainName sunib = DomainName(".","binus","ac","id");

    myDns->bindNewDomain(sunib,{192,168,0,1});
    // myDns->bindNewDomain(DomainName("binusmaya","binus","ac","id"),{192,168,0,2});
    // myDns->bindNewDomain(DomainName("centre","binus","ac","id"),{192,168,0,3});
    // myDns->bindNewDomain(DomainName(".","facebook","","com"),{192,168,0,4});
    // myDns->bindNewDomain(DomainName("api","facebook","","com"),{192,168,0,5});
    // myDns->bindNewDomain(DomainName(".","google","","id"),{192,168,0,6});
    result = myDns->resolve(sunib, false);
    // myDns->import_data("wkwk.txt");

    // Interface
    int choice  = -1;
    while( choice == -1 ){
        while ( choice < 1 || choice > 4)
        {
            system("cls");
            printMenu();
            cout << "Choose Menu: ";
            cin >> choice;
        }
        if( choice == 4 )return 0;

        else if( choice == 1 ){
            //resolve
            string sub = "", name = "", sld = "", tld = "";
            cout << "Note: No SLD input '.', No subdomain input '.'!" << endl;
            cout << "Enter domain name (format: sub name sld tld): " << endl;
            cin >> sub >> name >> sld >> tld;
            if( sld == "." )sld = "";
            DomainName target = DomainName(sub,name,sld,tld);
            result = myDns->resolve(target, true);
            cout << endl;
        }
        
        else if( choice == 2 ){
            //register
            string sub = "", name = "", sld = "", tld = "";
            cout << "Note: No SLD input '.', No subdomain input '.'!" << endl;
            cout << "Enter domain name (format: sub name sld tld): " << endl;
            cin >> sub >> name >> sld >> tld;
            if( sld == "." )sld = "";

            DomainName newDomain = DomainName(sub,name,sld,tld);
            ipaddress newIp;

            cout << "Note: Only enter 8bit int value!" << endl;
            cout << "Enter IP address (example: 192 168 0 1): " << endl;
            cin >> newIp.value[0] >> newIp.value[1] >> newIp.value[2] >> newIp.value[3];
            myDns->bindNewDomain(newDomain,newIp);
        }

        else if( choice == 3 ){
            //print all
            myDns->printAllTreeRecord();
        }

        anyToContinue();
        choice = -1;
    }

    return 0;
}
