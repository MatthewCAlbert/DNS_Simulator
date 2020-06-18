#include "DNS.h"

void printMenu(){
    printf("DNS Server Simulator\n");
    printf("====================\n");
    printf("|1. Resolve Name   |\n");
    printf("|2. Register IP    |\n");
    printf("|3. Exit           |\n");
    printf("====================\n");
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
    ipaddress result = myDns->resolve(sunib, false);

    // Interface
    /*
    *   Maybe upcoming feature:
    *   - Directory exploration
    *   - Unbind IP
    *   - Delete certain server until its children
    */
    int choice  = -1;
    while( choice == -1 ){
        while ( choice < 1 || choice > 3)
        {
            printMenu();
            cout << "Choose Menu: ";
            cin >> choice;
        }
        if( choice == 3 )return 0;

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

        choice = -1;
    }

    return 0;
}
