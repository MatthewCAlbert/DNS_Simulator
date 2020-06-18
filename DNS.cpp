#include "DNS.h"

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
    ipaddress result = myDns->resolve(sunib, true);
    return 0;
}
