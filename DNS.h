#include<stdlib.h> 
#include<stdio.h> 
#include<vector>
#include<map>
#include<cstring> 
#include<memory> 
#include<algorithm> 
#include<iterator> 
#include<regex> 
#include<iostream>
#include<fstream>

using namespace std;

// Helper function
// Case-insensitive string comparator
bool iequals(const string& a, const string& b)
{
    unsigned int sz = a.size();
    if (b.size() != sz)
        return false;
    for (unsigned int i = 0; i < sz; ++i)
        if (tolower(a[i]) != tolower(b[i]))
            return false;
    return true;
}

struct ipaddress
{
    uint8_t value[4]; 
};

// Compare IP address
bool ipCompare(ipaddress ip1, ipaddress ip2)
{
    for(int i = 0 ; i < 4 ; i++){
        if( ip1.value[i] != ip2.value[0] ) return false;
    }
    return true;
}

vector <string> split(string s, string delimiter){
    vector <string> res;
    size_t pos = 0;
    string token;
    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        res.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    res.push_back(s);
    return res;
}

//Helper function to print ipaddress datatypes
void printIP(ipaddress ip)
{
    printf("%i",ip.value[0]);
    for(int i = 1 ; i < 4 ; i++)
    {
        printf(".%i",ip.value[i]);
    }
}

class DomainName
{
    public:
    string subdomain;
    string name; 
    string sld;
    string tld;

    //'*' reserved for future use
    DomainName(string subdomain, string name, string sld, string tld)
    {
        //reserved
        bool regex_pass = true;
        // regex star_escape("^[a-zA-Z0-9_]*$|^[\\.\\*]{1}$"); 

        //escape char max 253 char
        if( subdomain.length() + name.length() + sld.length() + tld.length() > 250 ) regex_pass = false;

        //must be alphabet except . and *
        regex_pass = regex_match(subdomain, regex("^[a-zA-Z0-9_]*$|^[\.]{1}$"));
        this->subdomain = subdomain;

        //must be alphabet except *
        regex_pass = regex_match(name, regex("^[a-zA-Z0-9_]*$"));
        this->name = name;

        //must be alphabet except *
        regex_pass = !( !regex_match(sld, regex("^[a-zA-Z]*$")) || !regex_match(tld, regex("^[a-zA-Z]*$")) );
        this->sld = sld;
        this->tld = tld;
        if( !regex_pass ){printf("Invalid regex error!\n");}
    }

    void printDomain()
    {
        if( this->subdomain != "." ) cout << this->subdomain << ".";
        cout << this->name << ".";
        if( this->sld != "" ) cout << this->sld << ".";
        cout << this->tld;
    }
};

class NameServer
{
    private:
    map <string, ipaddress> resolver;
    
    public:
    ipaddress resolve( DomainName target )
    {
        for (auto const& x : resolver)
        {
            if(  iequals(x.first, target.subdomain)  ){
                return x.second;
            }
        }
        return {0,0,0,0};
    }
    
    int recordCount()
    {
        return resolver.size();
    }

    void bindIP(string key, ipaddress ip)
    {
        resolver.insert({key, ip});
    }

    void replaceIP(string key, ipaddress ip)
    {
        for (auto const& x : resolver)
        {
            if(  iequals(x.first, key)  ){
                resolver[key] = ip;
                return;
            }
        }
    }

    void deleteIP(string key){
        resolver.erase(key);
    }

    map <string, ipaddress> getAllRegisteredIp()
    {
        return resolver;
    }
};

class SLDServer
{
    private:
    map <string, shared_ptr<NameServer>> resolver;

    public:
    shared_ptr<NameServer> resolve( DomainName target )
    {
        for (auto const& x : resolver)
        {
            if( iequals(x.first, target.name) ){
                return x.second;
            }
        }
        return NULL;
    }
    
    int recordCount()
    {
        return resolver.size();
    }

    void bindNameServer(string key, shared_ptr<NameServer> nserv)
    {
        resolver.insert({key, nserv});
    }

    void registerNewNameServer(string key)
    {
        shared_ptr<NameServer> nserv = make_shared<NameServer>();
        resolver.insert({key, nserv});
    }

    void deleteNameServer(string key)
    {
        resolver.erase(key);
    }

    map <string, shared_ptr<NameServer>> getAllRegisteredNameServer()
    {
        return resolver;
    }
};

class TLDServer
{
    private:
    map <string, shared_ptr<SLDServer>> second;
    map <string, shared_ptr<NameServer>> resolver;

    public:
    shared_ptr<NameServer> resolve( DomainName target )
    {
        for (auto const& x : resolver)
        {
            if( x.first == target.name ){
                return x.second;
            }
        }
        return NULL;
    }

    shared_ptr<SLDServer> resolveSLD( DomainName target )
    {
        for (auto const& x : second)
        {
            if( iequals(x.first, target.sld) ){
                return x.second;
            }
        }
        return NULL;
    }
    
    int recordCount()
    {
        return resolver.size();
    }
    
    int SldCount()
    {
        return second.size();
    }

    void bindNameServer(string key, shared_ptr<NameServer> nserv)
    {
        resolver.insert({key, nserv});
    }

    void registerNewNameServer(string key)
    {
        shared_ptr<NameServer> nserv = make_shared<NameServer>();
        resolver.insert({key, nserv});
    }

    void registerNewSLDServer(string key)
    {
        shared_ptr<SLDServer> nserv = make_shared<SLDServer>();
        second.insert({key, nserv});
    }

    void deleteSLDServer(string key)
    {
        second.erase(key);
    }

    void deleteNameServer(string key)
    {
        resolver.erase(key);
    }

    map <string, shared_ptr<NameServer>> getAllRegisteredNameServer()
    {
        return resolver;
    }

    map <string, shared_ptr<SLDServer>> getAllRegisteredSLD()
    {
        return second;
    }
};

class RootServer
{
    private:
    map <string, shared_ptr<TLDServer>> resolver;

    public:
    shared_ptr<TLDServer> resolve( DomainName target )
    {
        for (auto const& x : resolver)
        {
            if(  iequals(x.first, target.tld) ){
                return x.second;
            }
        }
        return NULL;
    }
    
    int recordCount()
    {
        return resolver.size();
    }

    void bindTLDServer(string key, shared_ptr<TLDServer> tldserv)
    {
        resolver.insert({key, tldserv});
    }

    void registerNewTLDServer(string key)
    {
        shared_ptr<TLDServer> tldserv = make_shared<TLDServer>();
        resolver.insert({key, tldserv});
    }

    void deleteTLDServer(string key){
        resolver.erase(key);
    }

    map <string, shared_ptr<TLDServer>> getAllRegisteredTLD()
    {
        return resolver;
    }
};

class DNS
{
    private:
    shared_ptr <RootServer> rootserv;

    public:
    DNS(shared_ptr <RootServer> rootserv)
    {
        this->rootserv = rootserv;
    }

    ipaddress resolve( DomainName target, bool verbose = false )
    {
        shared_ptr<TLDServer> target_tld = this->rootserv->resolve(target);
        if( target_tld == NULL ) return {0,0,0,0};

        shared_ptr<SLDServer> target_sld;
        shared_ptr<NameServer> target_ns;
        if( target.sld != "" ){
            //have SLD
            target_sld = target_tld->resolveSLD(target);
            if( target_sld == NULL ) return {0,0,0,0};

            target_ns = target_sld->resolve(target);
        }else{
            //no SLD
            target_ns = target_tld->resolve(target);
        }
        if( target_ns == NULL ) return {0,0,0,0};
        ipaddress res = target_ns->resolve(target);
        if(verbose){
             printIP(res); cout << " "; target.printDomain();
        }
        return res;
    }

    void bindNewDomain( DomainName domain, ipaddress ip, bool overwrite = false)
    {
        if( domain.name == "*" || domain.subdomain == "*" || domain.sld == "*" || domain.tld == "*" ){printf("Cannot bind asterisk!\n");return;}
        shared_ptr<TLDServer> target_tld = this->rootserv->resolve(domain);
        shared_ptr<SLDServer> target_sld;
        shared_ptr<NameServer> target_ns;
        if( target_tld == NULL ){
            //create new TLDserver
            this->rootserv->registerNewTLDServer(domain.tld);
            target_tld = this->rootserv->resolve(domain);
        }

        if( domain.sld != "" ){
            //if have SLD
            target_sld = target_tld->resolveSLD(domain);
            if( target_sld == NULL ){
                //create new SLDserver
                target_tld->registerNewSLDServer(domain.sld);
                target_sld = target_tld->resolveSLD(domain);
            }
        }else{
            target_ns = target_tld->resolve(domain);
        }

        if( target_ns == NULL ){
            //create new NSserver
            if( domain.sld != "" ){
                target_sld->registerNewNameServer(domain.name);
                target_ns = target_sld->resolve(domain);
            }else{
                target_tld->registerNewNameServer(domain.name);
                target_ns = target_tld->resolve(domain);
            }
        }

        if( ipCompare(target_ns->resolve(domain),{0,0,0,0}) ){
            //bind IP
            target_ns->bindIP(domain.subdomain,ip);
            return;
        }else if( overwrite ){
            target_ns->replaceIP(domain.subdomain, ip);
        }
        printf("Domain already registered\n");
        return;
    }

    void deleteDomain( DomainName domain)
    {
        if( domain.name == "*" || domain.subdomain == "*" || domain.sld == "*" || domain.tld == "*" ){printf("Cannot bind asterisk!\n");return;}
        shared_ptr<TLDServer> target_tld = this->rootserv->resolve(domain);
        shared_ptr<SLDServer> target_sld;
        shared_ptr<NameServer> target_ns;
        if( target_tld == NULL ) return;

        if( domain.sld != "" ){
            //if have SLD
            target_sld = target_tld->resolveSLD(domain);
            if( target_sld == NULL ) return;
        }else{
            target_ns = target_tld->resolve(domain);
        }

        if( target_ns == NULL ) return;

        if( ipCompare(target_ns->resolve(domain),{0,0,0,0}) ) return;
        
        //found unbind IP from NS
        target_ns->deleteIP(domain.subdomain);

        //delete NS if empty
        if( target_ns->recordCount() == 0 ){
            if( domain.sld != "" ){
                //have sld
                target_sld->deleteNameServer(domain.name);
            }else{
                //no sld
                target_tld->deleteNameServer(domain.name);
            }
        }

        //delete SLD if empty & exist
        if( domain.sld != ""){
            if( target_sld->recordCount() == 0 ){
                target_tld->deleteSLDServer(domain.sld);
            }
        }

        //delete TLD if SLD and NS empty
        if( target_tld->recordCount() == 0 && target_tld->SldCount() == 0 ){
            this->rootserv->deleteTLDServer(domain.tld);
        }

        return;
    }

    void import_data(string source_dir, bool overwrite = true)
    {
        ifstream infile(source_dir);
        string ip, domain;

        //format: 0.0.0.0 !.domain.!.com
        while (infile >> ip >> domain)
        {
            //fetch IP Address
            ipaddress newIp;
            vector <string> splitIp = split(ip, ".");
            for( int i = 0 ; i < 4 ; i++ ) newIp.value[i] = stoi(splitIp[i]);

            //fetch Domain Name
            string domPart[4];
            vector <string> splitDom = split(domain, ".");
            for( int i = 0 ; i < 4 ; i++ ) domPart[i] = splitDom[i];
            domPart[0] = domPart[0] == "!" ? "." : domPart[0]; //sub
            domPart[2] = domPart[2] == "!" ? "" : domPart[2]; //sld
            DomainName newDomain = DomainName(domPart[0],domPart[1],domPart[2],domPart[3]);

            this->bindNewDomain(newDomain,newIp,overwrite);
        }
        infile.close();
        cout << "DNS Data Imported" << endl;
    }

    void export_data(string target_dir)
    {
        ofstream savefile;
        savefile.open(target_dir);
        map <string, shared_ptr<TLDServer>> a_tld = this->rootserv->getAllRegisteredTLD();
        
        //explore all TLD
        for (auto const& x : a_tld)
        {
            map <string, shared_ptr<SLDServer>> a_sld = x.second->getAllRegisteredSLD();

            //explore all SLD 
            for (auto const& y : a_sld)
            {
                map <string, shared_ptr<NameServer>> a_ns = y.second->getAllRegisteredNameServer();

                //explore current SLD NS
                for (auto const& z : a_ns)
                {
                    map <string, ipaddress> a_ip = z.second->getAllRegisteredIp();

                    //get all IP below SLD
                    for (auto const& ab : a_ip){
                        //write ip
                        string s_ip = "";
                        for( int i = 0 ; i < 3 ; i++ ) s_ip += to_string(ab.second.value[i])+".";
                        s_ip += to_string(ab.second.value[3]);

                        //write domain
                        string sub = ab.first == "." ? "!" : ab.first;
                        string s_dom = sub + "." + z.first + "." + y.first + "." + x.first;
                        savefile << s_ip+" "+s_dom << endl;
                    }
                }
            }

            //explore current TLD NS
            map <string, shared_ptr<NameServer>> a_ns = x.second->getAllRegisteredNameServer();
            for (auto const& y : a_ns)
            {
                map <string, ipaddress> a_ip = y.second->getAllRegisteredIp();

                //get all IP below TLD
                for (auto const& z : a_ip){
                    //write ip
                    string s_ip = "";
                    for( int i = 0 ; i < 3 ; i++ ) s_ip += to_string(z.second.value[i])+".";
                    s_ip += to_string(z.second.value[3]);

                    //write domain
                    string sub = z.first == "." ? "!" : z.first;
                    string s_dom = sub + "." + y.first + ".!." + x.first;
                    savefile << s_ip+" "+s_dom << endl;
                }
            }
        }
        savefile.close();
        cout << "DNS Data Saved" << endl;
    }

    void printAllTreeRecord()
    {
        cout << "==============" << endl;
        cout << "| All Record |" << endl;
        cout << "==============" << endl;
        map <string, shared_ptr<TLDServer>> a_tld = this->rootserv->getAllRegisteredTLD();
        
        //explore all TLD
        for (auto const& x : a_tld)
        {
            cout << x.first << endl; //print TLD
            map <string, shared_ptr<SLDServer>> a_sld = x.second->getAllRegisteredSLD();

            //explore current TLD NS
            cout << "|-." << endl;
            map <string, shared_ptr<NameServer>> a_ns = x.second->getAllRegisteredNameServer();
            for (auto const& y : a_ns)
            {
                cout << "  |-" << y.first << endl; //print TLD root
                map <string, ipaddress> a_ip = y.second->getAllRegisteredIp();

                //get all IP below TLD
                for (auto const& z : a_ip){
                    //write ip
                    string s_ip = "";
                    for( int i = 0 ; i < 3 ; i++ ) s_ip += to_string(z.second.value[i])+".";
                    s_ip += to_string(z.second.value[3]);

                    cout << "    |-" << z.first << ":" << s_ip << endl; //print sub
                }
            }

            //explore all SLD 
            for (auto const& y : a_sld)
            {
                cout << "|-" << y.first << endl; //print SLD
                map <string, shared_ptr<NameServer>> a_ns = y.second->getAllRegisteredNameServer();

                //explore current SLD NS
                for (auto const& z : a_ns)
                {
                    cout << "  |-" << z.first << endl; //print SLD root
                    map <string, ipaddress> a_ip = z.second->getAllRegisteredIp();

                    //get all IP below SLD
                    for (auto const& ab : a_ip){
                        //write ip
                        string s_ip = "";
                        for( int i = 0 ; i < 3 ; i++ ) s_ip += to_string(ab.second.value[i])+".";
                        s_ip += to_string(ab.second.value[3]);

                        //write domain
                        string sub = ab.first == "." ? "" : (ab.first+".");

                        
                        cout << "    |-" << ab.first << ":" << s_ip << endl; //print sub
                    }
                }
            }

        }
    }
};
