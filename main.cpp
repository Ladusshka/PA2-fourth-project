#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <functional>
#endif /* __PROGTEST__ */

class CNetwork;
class CComputer;
class CComponent{
public:
    virtual ~CComponent(){}
    virtual std::ostream & output(std::ostream & os, std::list<bool> flags)const = 0;
    virtual CComponent* copy()const =0;
    virtual CComputer * findComputer(const char *  Computer_name) =0;
    virtual CNetwork * findNetwork(const char *  Network_name)=0;

};
enum class ADDINGtype{
    DETAIL,
    IP
};

class CComputer:public CComponent{
public:
    explicit CComputer(const std::string& Computer_name):computer_name(std::string(Computer_name)){}
    CComputer & addAddress(std::string&& new_ip) {
        ip_addresses.push_back(std::move(new_ip));
        return *this;
    }
    CComputer& operator=(const CComputer& other) {
        if (this != &other) {
            CComputer temp(other);
            swap(*this, temp);
        }
        return *this;
    }

    CComputer duplicate(const std::pair<std::string, std::string> & remap);
     std::string getName() const {
        return computer_name;
    }

     void setName(const std::string& name)  {
        computer_name = name;
    }

    CComputer * findComputer(const char *  Computer_name) override{
        if(computer_name == std::string (Computer_name)){
            return this;
        }
        for(size_t i=0;i<details.size();i++){
            auto tmp = details[i]->findComputer(Computer_name);
            if(tmp!= nullptr){
                return tmp;
            }
        }
        return nullptr;
    }



    CNetwork * findNetwork(const char *  Network_name) override{
        for(size_t i=0;i<details.size();i++){
            auto tmp = details[i]->findNetwork(Network_name);
            if(tmp!= nullptr){
                return tmp;
            }
        }
        return nullptr;
    }

    CComputer(const CComputer& other)
                                    // Third, initialized empty and then filled
    {
         // First as declared
         ip_addresses = other.ip_addresses ;  // Second as declared
        details.reserve(other.details.size());
        for (const auto& comp : other.details) {
            details.push_back(std::shared_ptr<CComponent>(comp->copy()));
        }
        computer_name = other.computer_name;
    }


    friend void swap(CComputer& first, CComputer& second) {
        using std::swap;
        swap(first.computer_name, second.computer_name);
        swap(first.ip_addresses, second.ip_addresses);
        swap(first.details, second.details);
    }

    CComputer& addComponent(const CComponent& detail) {
        details.push_back(std::shared_ptr<CComponent>(detail.copy()));
        return *this;
    }
    const std::string & get_name()const {
        return computer_name;
    }
    CComponent* copy() const override {
        CComputer* newComputer = new CComputer(computer_name.c_str());
        newComputer->ip_addresses = this->ip_addresses;
        for (const auto& detail : this->details) {
            newComputer->details.push_back(std::shared_ptr<CComponent>(detail->copy()));
        }
        return newComputer;
    }
    CComputer duplicate(const std::vector<std::pair<std::string, std::string>> & remap);
    friend  std::ostream & operator << (std::ostream & os, const CComputer & comp){
        //"Host: 00581251.kicbvk.cz\n"

        os<<"Host: "<<comp.computer_name<<"\n";
        for(size_t i=0;i<comp.ip_addresses.size();i++){
            os<<"+-"<<comp.ip_addresses[i]<<"\n";
        }
        std::list<bool> flags;
        flags.push_back(true);
        if(!comp.details.empty()){
            for(size_t i=0;i<comp.details.size()-1;i++){
                comp.details[i]->output(os,flags);
            }
            flags.pop_back();
            flags.push_back(false);
            comp.details[comp.details.size()-1]->output(os,flags);
        }

        return os;

    }
    std::ostream & output(std::ostream & os, std::list<bool> flags)const override{

            auto it = flags.begin();
            for ( it = flags.begin(); it != --flags.end(); ++it) {
                if(*it){
                    os<<"|";
                }else{
                    os<<" ";
                }
            }
            if(*it){
                os<<"+";
            }else{
                os<<"\\";
            }
            //+-Host: monitor4.fit.cvut.cz\n"
            os<<"-Host: "<<computer_name<<"\n";
            /*"    +-Host: monitor5.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"*/
            flags.push_back(false);
            flags.push_back(true);
            for(size_t i=0;i<ip_addresses.size();i++){
                auto it2 = flags.begin();
                for ( it2 = flags.begin(); it2 != --flags.end(); ++it2) {
                    if(*it2){
                        os<<"|";
                    }else{
                        os<<" ";
                    }
                }
                if(*it2){
                    os<<"+";
                }else{
                    os<<"\\";
                }
                os<<"-"<<ip_addresses[i]<<"\n";
            }

            if(!details.empty()){
                for(size_t i= 0 ;i<details.size()-1;i++){
                    details[i]->output(os,flags);
                }
                flags.pop_back();
                flags.push_back(false);
                details[details.size()-1]->output(os,flags);
            }
            flags.pop_back();
            flags.pop_back();

        return os;
    }

private:
    std::vector<std::string> ip_addresses;
    std::vector<std::shared_ptr<CComponent>> details;
    std::string computer_name;


};
struct CompComputer{
    bool operator ()(const std::shared_ptr<CComputer> & left, const std::shared_ptr<CComputer> & right)const{
        return left->get_name() < right->get_name();
    }
};
class CNetwork : public CComponent{
public:
    explicit CNetwork(const std::string& name) : network_name(name) {}

    CNetwork & addComputer(const CComputer & computer){
        auto x = std::make_shared<CComputer>(computer);
        computers.push_back(x);

        return *this;
    }

    CNetwork * findNetwork(const char *  Network_name) override{
        if(network_name == std::string (Network_name)){
            return this;
        }
        for(size_t i =0 ;i<computers.size();i++){
            auto tmp = computers[i]->findNetwork(Network_name);
            if(tmp!= nullptr){
                return tmp;
            }
        }
        return nullptr;
    }
    CComputer* findComputer(const char * name)  override{
       for(size_t i=0;i<computers.size();i++){
           auto tmp = computers[i]->findComputer(name);
           if(tmp!= nullptr){
               return tmp;
           }
       }
       return nullptr;
    }
    CNetwork& operator=(const CNetwork& other) {
        if (this == &other) return *this;
        CNetwork temp(other);
        swap(*this, temp);
        return *this;
    }
    friend std::ostream & operator << (std::ostream & os,const CNetwork & net){
        //"Network: FIT network\n"
        os<<"Network: "<<net.network_name<<"\n";
        if(net.computers.size() > 1){
            std::list<bool> flags;
            flags.push_back(true);
            for(size_t i =0;i<net.computers.size()-1;i++){
                net.computers[i]->output(os,flags);
            }
            flags.pop_back();
            flags.push_back(false);

            net.computers[net.computers.size()-1]->output(os,flags);
        }else if(net.computers.size() == 1){
            std::list<bool> flags;
            flags.push_back(false);
            net.computers[net.computers.size()-1]->output(os,flags);
        }
        return os;
    }
    friend void swap(CNetwork& first, CNetwork& second) {
        using std::swap;
        swap(first.network_name, second.network_name);
        swap(first.computers, second.computers);

    }

    CNetwork(const CNetwork& other)
            : network_name(other.network_name) {
        computers.reserve(other.computers.size());
        for (const auto& computer : other.computers) {
            std::shared_ptr<CComputer> newComp = std::make_shared<CComputer>(*computer);
            computers.push_back(newComp);

        }
    }

    const std::string & get_name()const{
        return network_name;
    }
    void set_name(std::string & new_name){
        network_name =std::string(new_name);
    }

    CComponent* copy() const override {

        CNetwork* newNetwork = new CNetwork(network_name.c_str());

        for (const auto& computer : computers) {
            auto copiedComputer = std::shared_ptr<CComputer>(dynamic_cast<CComputer*>(computer->copy()));
            newNetwork->computers.push_back(copiedComputer);

        }

        return newNetwork;
    }


    std::ostream & output(std::ostream & os, std::list<bool> flags)const override{

        auto it = flags.begin();
        for ( it = flags.begin(); it != --flags.end(); ++it) {
            if(*it){
                os<<"|";
            }else{
                os<<" ";
            }
        }
        if(*it){
            os<<"+";
        }else{
            os<<"\\";
        }
        /* "  +-Network: dummy00\n"
              "  | \\-Host: testing.fit.cvut.cz\n"
              "  |   +-192.168.1.1\n"
              "  |   +-CPU, 1 cores @ 300MHz\n"
              "  |   \\-Memory, 256 MiB\n"
              "  +-Memory, 16384 MiB\n"
              "  \\-Network: vnet00\n"
              "    +-Host: monitor3.fit.cvut.cz\n"
              "    | +-147.32.232.254\n"
              "    | +-CPU, 2 cores @ 1500MHz\n"
              "    | +-Memory, 4096 MiB\n"
              "    | \\-HDD, 750 GiB\n"
              "    |   +-[0]: 100 GiB, root\n"
              "    |   \\-[1]: 600 GiB, log\n"
              "    \\-Host: monitor4.fit.cvut.cz\n"
              "      +-147.32.232.254\n"
              "      +-CPU, 2 cores @ 1500MHz\n"
              "      +-Memory, 4096 MiB\n"
              "      \\-HDD, 750 GiB\n"
              "        +-[0]: 100 GiB, root\n"
              "        \\-[1]: 600 GiB, log\n" );*/
        os<<"-Network: "<<network_name<<"\n";
        flags.push_back(false);
        flags.push_back(true);

        if(!computers.empty()){
            for(size_t i= 0 ;i<computers.size()-1;i++){
                computers[i]->output(os,flags);
            }
            flags.pop_back();
            flags.push_back(false);
            computers[computers.size()-1]->output(os,flags);
        }
        flags.pop_back();
        flags.pop_back();

        return os;
    }

private:
    std::string network_name;
    std::vector<std::shared_ptr<CComputer>> computers;

};


class CCPU : public CComponent{
public:
    CCPU(int nc, int fr):number_cores(nc),frequency(fr){}
    CCPU* copy() const override {
        return new CCPU(this->number_cores, this->frequency);
    }
    CNetwork * findNetwork(const char *  Network_name) override{
        return nullptr;
    }
    CComputer * findComputer(const char *  Computer_name) override{
        return nullptr;
    }


    std::ostream & output(std::ostream & os, std::list<bool> flags)const override{

        auto it = flags.begin();
        for ( it = flags.begin(); it != --flags.end(); ++it) {
            if(*it){
                os<<"|";
            }else{
                os<<" ";
            }
        }
        if(*it){
            os<<"+";
        }else{
            os<<"\\";
        }
        os<<"-CPU, "<<number_cores<<" cores @ "<<frequency<<"MHz\n";
        return os;
    }

private:
    int number_cores;
    int frequency;
};





class CMemory : public CComponent{
public:
    CMemory(int RAM) : ram(RAM) {}

    CMemory *copy() const override {
        return new CMemory(this->ram);
    }
    CComputer * findComputer(const char *  Computer_name) override{
        return nullptr;
    }


    std::ostream & output(std::ostream & os, std::list<bool> flags)const override{
        auto it = flags.begin();
        for ( it = flags.begin(); it != --flags.end(); ++it) {
            if(*it){
                os<<"|";
            }else{
                os<<" ";
            }
        }
        if(*it){
            os<<"+";
        }else{
            os<<"\\";
        }
        os<<"-Memory, "<<ram<<" MiB\n";
        return os;
    }
    CNetwork * findNetwork(const char *  Network_name) override{
        return nullptr;
    }
private:
    int ram;
};








struct partition{
public:
    partition(std::string && a, int b):part_name(a), part_size(b){}
    std::string part_name;
    int part_size;
};
class CDisk : public CComponent{
public:
    enum Type {MAGNETIC, SSD };
    CDisk(Type type, int Capacity):type(type), capacity(Capacity){}

    CComputer * findComputer(const char *  Computer_name) override{
        return nullptr;
    }


    CDisk * copy()const override{
        CDisk* newDisk = new CDisk(type, capacity);
        for (const auto& part : partitions) {
            newDisk->addPartition(part.part_size, part.part_name);
        }
        return newDisk;
    }
    CNetwork * findNetwork(const char *  Network_name) override{
        return nullptr;
    }
    CDisk & addPartition(int size, std::string name){
        partitions.emplace_back(std::move(name),size);
        return *this;
    }
    std::ostream & output(std::ostream & os, std::list<bool> flags)const override{

            auto it = flags.begin();

            for ( it = flags.begin(); it != --flags.end(); ++it) {
                if(*it){
                    os<<"|";
                }else{
                    os<<" ";
                }
            }
            if(*it){
                os<<"+";

            }else{
                os<<"\\";
            }
            if(type == Type::MAGNETIC){
                os<<"-HDD, ";
            }
            else{
                os<<"-SSD, ";
            }
            os<<capacity<<" GiB\n";


            if(!partitions.empty()){
                flags.push_back(false);
                flags.push_back(false);
                for(size_t i=0;i<partitions.size()-1;i++){
                    auto it2 = flags.begin();
                    for ( it2 = flags.begin(); it2 != --flags.end(); ++it2) {
                        if(*it2){
                            os<<"|";
                        }else{
                            os<<" ";
                        }

                    }os<<"+-["<<i<<"]: "<<partitions[i].part_size<<" GiB, "<<partitions[i].part_name<<"\n";

                }
                auto it2 = flags.begin();
                for ( it2 = flags.begin(); it2 != --flags.end(); ++it2) {
                    if(*it2){
                        os<<"|";
                    }else{
                        os<<" ";
                    }


                }os<<"\\-["<<partitions.size()-1<<"]: "<<partitions[partitions.size()-1].part_size<<" GiB, "<<partitions[partitions.size()-1].part_name<<"\n";
                flags.pop_back();
                flags.pop_back();

        }
        return os;
    }

private:
    Type type;
    int capacity;
    std::vector<partition> partitions;

};


CComputer CComputer::duplicate(const std::vector<std::pair<std::string, std::string>> & remap){
    CComputer result(*this);

    for(auto &a : remap){
        CNetwork* tmpnet = result.findNetwork(a.first.c_str());
        if(tmpnet!= nullptr){
            std::string copy = a.second;
            tmpnet->set_name(copy);
        }
        CComputer * tmpcomp = result.findComputer(a.first.c_str());
        if(tmpcomp!= nullptr){
            std::string copy = a.second;
            tmpcomp->setName(copy);
        }
    }

    return result;
}
CComputer CComputer::duplicate(const std::pair<std::string, std::string> & remap){
    CComputer result(*this);


        CNetwork* tmpnet = result.findNetwork(remap.first.c_str());
        if(tmpnet!= nullptr){
            std::string copy = remap.second;
            tmpnet->set_name(copy);
        }
        CComputer * tmpcomp = result.findComputer(remap.first.c_str());
        if(tmpcomp!= nullptr){
            std::string copy = remap.second;
            tmpcomp->setName(copy);
        }


    return result;
}
#ifndef __PROGTEST__
template<typename T_>
std::string toString ( const T_ & x )
{
    std::ostringstream oss;
    oss << x;
    return oss . str ();
}

int main ()
{
    CNetwork n ( "FIT network" );
    n . addComputer (
            CComputer ( "progtest.fit.cvut.cz" ) .
                    addAddress ( "147.32.232.142" ) .
                    addComponent ( CCPU ( 8, 2400 ) ) .
                    addComponent ( CCPU ( 8, 1200 ) ) .
                    addComponent ( CDisk ( CDisk::MAGNETIC, 1500 ) .
                    addPartition ( 50, "/" ) .
                    addPartition ( 5, "/boot" ).
                    addPartition ( 1000, "/var" ) ) .
                    addComponent ( CDisk ( CDisk::SSD, 60 ) .
                    addPartition ( 60, "/data" )  ) .
                    addComponent ( CMemory ( 2000 ) ).
                    addComponent ( CMemory ( 2000 ) ) ) .
            addComputer (
            CComputer ( "courses.fit.cvut.cz" ) .
                    addAddress ( "147.32.232.213" ) .
                    addComponent ( CCPU ( 4, 1600 ) ) .
                    addComponent ( CMemory ( 4000 ) ).
                    addComponent ( CDisk ( CDisk::MAGNETIC, 2000 ) .
                    addPartition ( 100, "/" )   .
                    addPartition ( 1900, "/data" ) ) ) .
            addComputer (
            CComputer ( "imap.fit.cvut.cz" ) .
                    addAddress ( "147.32.232.238" ) .
                    addComponent ( CCPU ( 4, 2500 ) ) .
                    addAddress ( "2001:718:2:2901::238" ) .
                    addComponent ( CMemory ( 8000 ) ) );
   // std::cout<<n<<std::endl;
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  \\-Memory, 8000 MiB\n" );
    CNetwork x = n;
    auto c = x . findComputer ( "imap.fit.cvut.cz" );
   // std::cout<<*c<<std::endl;
    assert ( toString ( *c ) ==
             "Host: imap.fit.cvut.cz\n"
             "+-147.32.232.238\n"
             "+-2001:718:2:2901::238\n"
             "+-CPU, 4 cores @ 2500MHz\n"
             "\\-Memory, 8000 MiB\n" );
    c -> addComponent ( CDisk ( CDisk::MAGNETIC, 1000 ) .
            addPartition ( 100, "system" ) .
            addPartition ( 200, "WWW" ) .
            addPartition ( 700, "mail" ) );
    assert ( toString ( x ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  +-Memory, 8000 MiB\n"
             "  \\-HDD, 1000 GiB\n"
             "    +-[0]: 100 GiB, system\n"
             "    +-[1]: 200 GiB, WWW\n"
             "    \\-[2]: 700 GiB, mail\n" );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "\\-Host: imap.fit.cvut.cz\n"
             "  +-147.32.232.238\n"
             "  +-2001:718:2:2901::238\n"
             "  +-CPU, 4 cores @ 2500MHz\n"
             "  \\-Memory, 8000 MiB\n" );
    n . addComputer ( CComputer ( "vm01.fit.cvut.cz" ) .
            addAddress ( "147.32.232.232" ) .
            addComponent ( CCPU ( 32, 4000 ) ) .
            addComponent ( CMemory ( 32768 ) ) .
            addComponent ( CDisk ( CDisk::MAGNETIC, 3000 ) .
            addPartition ( 500, "SYSTEM" ) .
            addPartition ( 2000, "DATA" ) .
            addPartition ( 500, "BACKUP" ) ) .
            addComponent ( CNetwork ( "dummy00" ) .
            addComputer ( CComputer ( "testing.fit.cvut.cz" ) .
            addAddress ( "192.168.1.1" ) .
            addComponent ( CCPU ( 1, 300 ) ) .
            addComponent ( CMemory ( 256 ) ) ) ) .
            addComponent ( CMemory ( 16384 ) ) .
            addComponent ( CNetwork ( "vnet00" ) ) );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "+-Host: imap.fit.cvut.cz\n"
             "| +-147.32.232.238\n"
             "| +-2001:718:2:2901::238\n"
             "| +-CPU, 4 cores @ 2500MHz\n"
             "| \\-Memory, 8000 MiB\n"
             "\\-Host: vm01.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet00\n" );

    auto vmnet = n . findNetwork ( "vnet00" );
    vmnet -> addComputer ( CComputer ( "monitor1.fit.cvut.cz" ) .
            addAddress ( "147.32.232.254" ) .
            addComponent ( CCPU ( 2, 1500 ) ) .
            addComponent ( CMemory ( 4096 ) ) .
            addComponent ( CDisk ( CDisk::MAGNETIC, 750 ) .
            addPartition ( 100, "root" ) .
            addPartition ( 600, "log" ) ) );
    //std::cout<<*vmnet<<std::endl;
    assert ( toString ( *vmnet ) ==
             "Network: vnet00\n"
             "\\-Host: monitor1.fit.cvut.cz\n"
             "  +-147.32.232.254\n"
             "  +-CPU, 2 cores @ 1500MHz\n"
             "  +-Memory, 4096 MiB\n"
             "  \\-HDD, 750 GiB\n"
             "    +-[0]: 100 GiB, root\n"
             "    \\-[1]: 600 GiB, log\n" );
     assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "+-Host: imap.fit.cvut.cz\n"
             "| +-147.32.232.238\n"
             "| +-2001:718:2:2901::238\n"
             "| +-CPU, 4 cores @ 2500MHz\n"
             "| \\-Memory, 8000 MiB\n"
             "\\-Host: vm01.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet00\n"
             "    \\-Host: monitor1.fit.cvut.cz\n"
             "      +-147.32.232.254\n"
             "      +-CPU, 2 cores @ 1500MHz\n"
             "      +-Memory, 4096 MiB\n"
             "      \\-HDD, 750 GiB\n"
             "        +-[0]: 100 GiB, root\n"
             "        \\-[1]: 600 GiB, log\n" );
    vmnet -> addComputer ( n . findComputer ( "monitor1.fit.cvut.cz" )
                                   -> duplicate ( { std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor2.fit.cvut.cz") } ) );
   assert ( toString ( *vmnet ) ==
             "Network: vnet00\n"
             "+-Host: monitor1.fit.cvut.cz\n"
             "| +-147.32.232.254\n"
             "| +-CPU, 2 cores @ 1500MHz\n"
             "| +-Memory, 4096 MiB\n"
             "| \\-HDD, 750 GiB\n"
             "|   +-[0]: 100 GiB, root\n"
             "|   \\-[1]: 600 GiB, log\n"
             "\\-Host: monitor2.fit.cvut.cz\n"
             "  +-147.32.232.254\n"
             "  +-CPU, 2 cores @ 1500MHz\n"
             "  +-Memory, 4096 MiB\n"
             "  \\-HDD, 750 GiB\n"
             "    +-[0]: 100 GiB, root\n"
             "    \\-[1]: 600 GiB, log\n" );
    n . addComputer ( n . findComputer ( "vm01.fit.cvut.cz" )
                              -> duplicate (
                                      {
                                              std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor3.fit.cvut.cz"),
                                              std::pair<std::string,std::string>("monitor2.fit.cvut.cz","monitor4.fit.cvut.cz"),
                                              std::pair<std::string,std::string>("vm01.fit.cvut.cz","vm02.fit.cvut.cz")
                                      } ) );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "+-Host: imap.fit.cvut.cz\n"
             "| +-147.32.232.238\n"
             "| +-2001:718:2:2901::238\n"
             "| +-CPU, 4 cores @ 2500MHz\n"
             "| \\-Memory, 8000 MiB\n"
             "+-Host: vm01.fit.cvut.cz\n"
             "| +-147.32.232.232\n"
             "| +-CPU, 32 cores @ 4000MHz\n"
             "| +-Memory, 32768 MiB\n"
             "| +-HDD, 3000 GiB\n"
             "| | +-[0]: 500 GiB, SYSTEM\n"
             "| | +-[1]: 2000 GiB, DATA\n"
             "| | \\-[2]: 500 GiB, BACKUP\n"
             "| +-Network: dummy00\n"
             "| | \\-Host: testing.fit.cvut.cz\n"
             "| |   +-192.168.1.1\n"
             "| |   +-CPU, 1 cores @ 300MHz\n"
             "| |   \\-Memory, 256 MiB\n"
             "| +-Memory, 16384 MiB\n"
             "| \\-Network: vnet00\n"
             "|   +-Host: monitor1.fit.cvut.cz\n"
             "|   | +-147.32.232.254\n"
             "|   | +-CPU, 2 cores @ 1500MHz\n"
             "|   | +-Memory, 4096 MiB\n"
             "|   | \\-HDD, 750 GiB\n"
             "|   |   +-[0]: 100 GiB, root\n"
             "|   |   \\-[1]: 600 GiB, log\n"
             "|   \\-Host: monitor2.fit.cvut.cz\n"
             "|     +-147.32.232.254\n"
             "|     +-CPU, 2 cores @ 1500MHz\n"
             "|     +-Memory, 4096 MiB\n"
             "|     \\-HDD, 750 GiB\n"
             "|       +-[0]: 100 GiB, root\n"
             "|       \\-[1]: 600 GiB, log\n"
             "\\-Host: vm02.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet00\n"
             "    +-Host: monitor3.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"
             "    \\-Host: monitor4.fit.cvut.cz\n"
             "      +-147.32.232.254\n"
             "      +-CPU, 2 cores @ 1500MHz\n"
             "      +-Memory, 4096 MiB\n"
             "      \\-HDD, 750 GiB\n"
             "        +-[0]: 100 GiB, root\n"
             "        \\-[1]: 600 GiB, log\n" );
    vmnet -> addComputer ( n . findComputer ( "vm02.fit.cvut.cz" )
                                   -> duplicate (
                                           {
                                                   std::pair<std::string,std::string>("monitor3.fit.cvut.cz","monitor5.fit.cvut.cz"),
                                                   std::pair<std::string,std::string>("monitor4.fit.cvut.cz","monitor6.fit.cvut.cz"),
                                                   std::pair<std::string,std::string>("vm02.fit.cvut.cz","vm03.fit.cvut.cz"),
                                                   std::pair<std::string,std::string>("vnet00","vnet01")
                                           } ) );
    assert ( toString ( *vmnet ) ==
             "Network: vnet00\n"
             "+-Host: monitor1.fit.cvut.cz\n"
             "| +-147.32.232.254\n"
             "| +-CPU, 2 cores @ 1500MHz\n"
             "| +-Memory, 4096 MiB\n"
             "| \\-HDD, 750 GiB\n"
             "|   +-[0]: 100 GiB, root\n"
             "|   \\-[1]: 600 GiB, log\n"
             "+-Host: monitor2.fit.cvut.cz\n"
             "| +-147.32.232.254\n"
             "| +-CPU, 2 cores @ 1500MHz\n"
             "| +-Memory, 4096 MiB\n"
             "| \\-HDD, 750 GiB\n"
             "|   +-[0]: 100 GiB, root\n"
             "|   \\-[1]: 600 GiB, log\n"
             "\\-Host: vm03.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet01\n"
             "    +-Host: monitor5.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"
             "    \\-Host: monitor6.fit.cvut.cz\n"
             "      +-147.32.232.254\n"
             "      +-CPU, 2 cores @ 1500MHz\n"
             "      +-Memory, 4096 MiB\n"
             "      \\-HDD, 750 GiB\n"
             "        +-[0]: 100 GiB, root\n"
             "        \\-[1]: 600 GiB, log\n" );
    n . findComputer ( "vm02.fit.cvut.cz" )
            -> findNetwork ( "vnet00" )
            -> addComputer ( n . findComputer ( "vm01.fit.cvut.cz" )
                                     -> duplicate (
                                             {
                                                     std::pair<std::string,std::string>("monitor1.fit.cvut.cz","monitor11.fit.cvut.cz"),
                                                     std::pair<std::string,std::string>("monitor2.fit.cvut.cz","monitor12.fit.cvut.cz"),
                                                     std::pair<std::string,std::string>("vm01.fit.cvut.cz","vm11.fit.cvut.cz"),
                                                     std::pair<std::string,std::string>("vm03.fit.cvut.cz","vm13.fit.cvut.cz"),
                                                     std::pair<std::string,std::string>("vnet00","vnet10"),
                                                     std::pair<std::string,std::string>("vnet01","vnet11")
                                             } ) );
    assert ( toString ( n ) ==
             "Network: FIT network\n"
             "+-Host: progtest.fit.cvut.cz\n"
             "| +-147.32.232.142\n"
             "| +-CPU, 8 cores @ 2400MHz\n"
             "| +-CPU, 8 cores @ 1200MHz\n"
             "| +-HDD, 1500 GiB\n"
             "| | +-[0]: 50 GiB, /\n"
             "| | +-[1]: 5 GiB, /boot\n"
             "| | \\-[2]: 1000 GiB, /var\n"
             "| +-SSD, 60 GiB\n"
             "| | \\-[0]: 60 GiB, /data\n"
             "| +-Memory, 2000 MiB\n"
             "| \\-Memory, 2000 MiB\n"
             "+-Host: courses.fit.cvut.cz\n"
             "| +-147.32.232.213\n"
             "| +-CPU, 4 cores @ 1600MHz\n"
             "| +-Memory, 4000 MiB\n"
             "| \\-HDD, 2000 GiB\n"
             "|   +-[0]: 100 GiB, /\n"
             "|   \\-[1]: 1900 GiB, /data\n"
             "+-Host: imap.fit.cvut.cz\n"
             "| +-147.32.232.238\n"
             "| +-2001:718:2:2901::238\n"
             "| +-CPU, 4 cores @ 2500MHz\n"
             "| \\-Memory, 8000 MiB\n"
             "+-Host: vm01.fit.cvut.cz\n"
             "| +-147.32.232.232\n"
             "| +-CPU, 32 cores @ 4000MHz\n"
             "| +-Memory, 32768 MiB\n"
             "| +-HDD, 3000 GiB\n"
             "| | +-[0]: 500 GiB, SYSTEM\n"
             "| | +-[1]: 2000 GiB, DATA\n"
             "| | \\-[2]: 500 GiB, BACKUP\n"
             "| +-Network: dummy00\n"
             "| | \\-Host: testing.fit.cvut.cz\n"
             "| |   +-192.168.1.1\n"
             "| |   +-CPU, 1 cores @ 300MHz\n"
             "| |   \\-Memory, 256 MiB\n"
             "| +-Memory, 16384 MiB\n"
             "| \\-Network: vnet00\n"
             "|   +-Host: monitor1.fit.cvut.cz\n"
             "|   | +-147.32.232.254\n"
             "|   | +-CPU, 2 cores @ 1500MHz\n"
             "|   | +-Memory, 4096 MiB\n"
             "|   | \\-HDD, 750 GiB\n"
             "|   |   +-[0]: 100 GiB, root\n"
             "|   |   \\-[1]: 600 GiB, log\n"
             "|   +-Host: monitor2.fit.cvut.cz\n"
             "|   | +-147.32.232.254\n"
             "|   | +-CPU, 2 cores @ 1500MHz\n"
             "|   | +-Memory, 4096 MiB\n"
             "|   | \\-HDD, 750 GiB\n"
             "|   |   +-[0]: 100 GiB, root\n"
             "|   |   \\-[1]: 600 GiB, log\n"
             "|   \\-Host: vm03.fit.cvut.cz\n"
             "|     +-147.32.232.232\n"
             "|     +-CPU, 32 cores @ 4000MHz\n"
             "|     +-Memory, 32768 MiB\n"
             "|     +-HDD, 3000 GiB\n"
             "|     | +-[0]: 500 GiB, SYSTEM\n"
             "|     | +-[1]: 2000 GiB, DATA\n"
             "|     | \\-[2]: 500 GiB, BACKUP\n"
             "|     +-Network: dummy00\n"
             "|     | \\-Host: testing.fit.cvut.cz\n"
             "|     |   +-192.168.1.1\n"
             "|     |   +-CPU, 1 cores @ 300MHz\n"
             "|     |   \\-Memory, 256 MiB\n"
             "|     +-Memory, 16384 MiB\n"
             "|     \\-Network: vnet01\n"
             "|       +-Host: monitor5.fit.cvut.cz\n"
             "|       | +-147.32.232.254\n"
             "|       | +-CPU, 2 cores @ 1500MHz\n"
             "|       | +-Memory, 4096 MiB\n"
             "|       | \\-HDD, 750 GiB\n"
             "|       |   +-[0]: 100 GiB, root\n"
             "|       |   \\-[1]: 600 GiB, log\n"
             "|       \\-Host: monitor6.fit.cvut.cz\n"
             "|         +-147.32.232.254\n"
             "|         +-CPU, 2 cores @ 1500MHz\n"
             "|         +-Memory, 4096 MiB\n"
             "|         \\-HDD, 750 GiB\n"
             "|           +-[0]: 100 GiB, root\n"
             "|           \\-[1]: 600 GiB, log\n"
             "\\-Host: vm02.fit.cvut.cz\n"
             "  +-147.32.232.232\n"
             "  +-CPU, 32 cores @ 4000MHz\n"
             "  +-Memory, 32768 MiB\n"
             "  +-HDD, 3000 GiB\n"
             "  | +-[0]: 500 GiB, SYSTEM\n"
             "  | +-[1]: 2000 GiB, DATA\n"
             "  | \\-[2]: 500 GiB, BACKUP\n"
             "  +-Network: dummy00\n"
             "  | \\-Host: testing.fit.cvut.cz\n"
             "  |   +-192.168.1.1\n"
             "  |   +-CPU, 1 cores @ 300MHz\n"
             "  |   \\-Memory, 256 MiB\n"
             "  +-Memory, 16384 MiB\n"
             "  \\-Network: vnet00\n"
             "    +-Host: monitor3.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"
             "    +-Host: monitor4.fit.cvut.cz\n"
             "    | +-147.32.232.254\n"
             "    | +-CPU, 2 cores @ 1500MHz\n"
             "    | +-Memory, 4096 MiB\n"
             "    | \\-HDD, 750 GiB\n"
             "    |   +-[0]: 100 GiB, root\n"
             "    |   \\-[1]: 600 GiB, log\n"
             "    \\-Host: vm11.fit.cvut.cz\n"
             "      +-147.32.232.232\n"
             "      +-CPU, 32 cores @ 4000MHz\n"
             "      +-Memory, 32768 MiB\n"
             "      +-HDD, 3000 GiB\n"
             "      | +-[0]: 500 GiB, SYSTEM\n"
             "      | +-[1]: 2000 GiB, DATA\n"
             "      | \\-[2]: 500 GiB, BACKUP\n"
             "      +-Network: dummy00\n"
             "      | \\-Host: testing.fit.cvut.cz\n"
             "      |   +-192.168.1.1\n"
             "      |   +-CPU, 1 cores @ 300MHz\n"
             "      |   \\-Memory, 256 MiB\n"
             "      +-Memory, 16384 MiB\n"
             "      \\-Network: vnet10\n"
             "        +-Host: monitor11.fit.cvut.cz\n"
             "        | +-147.32.232.254\n"
             "        | +-CPU, 2 cores @ 1500MHz\n"
             "        | +-Memory, 4096 MiB\n"
             "        | \\-HDD, 750 GiB\n"
             "        |   +-[0]: 100 GiB, root\n"
             "        |   \\-[1]: 600 GiB, log\n"
             "        +-Host: monitor12.fit.cvut.cz\n"
             "        | +-147.32.232.254\n"
             "        | +-CPU, 2 cores @ 1500MHz\n"
             "        | +-Memory, 4096 MiB\n"
             "        | \\-HDD, 750 GiB\n"
             "        |   +-[0]: 100 GiB, root\n"
             "        |   \\-[1]: 600 GiB, log\n"
             "        \\-Host: vm13.fit.cvut.cz\n"
             "          +-147.32.232.232\n"
             "          +-CPU, 32 cores @ 4000MHz\n"
             "          +-Memory, 32768 MiB\n"
             "          +-HDD, 3000 GiB\n"
             "          | +-[0]: 500 GiB, SYSTEM\n"
             "          | +-[1]: 2000 GiB, DATA\n"
             "          | \\-[2]: 500 GiB, BACKUP\n"
             "          +-Network: dummy00\n"
             "          | \\-Host: testing.fit.cvut.cz\n"
             "          |   +-192.168.1.1\n"
             "          |   +-CPU, 1 cores @ 300MHz\n"
             "          |   \\-Memory, 256 MiB\n"
             "          +-Memory, 16384 MiB\n"
             "          \\-Network: vnet11\n"
             "            +-Host: monitor5.fit.cvut.cz\n"
             "            | +-147.32.232.254\n"
             "            | +-CPU, 2 cores @ 1500MHz\n"
             "            | +-Memory, 4096 MiB\n"
             "            | \\-HDD, 750 GiB\n"
             "            |   +-[0]: 100 GiB, root\n"
             "            |   \\-[1]: 600 GiB, log\n"
             "            \\-Host: monitor6.fit.cvut.cz\n"
             "              +-147.32.232.254\n"
             "              +-CPU, 2 cores @ 1500MHz\n"
             "              +-Memory, 4096 MiB\n"
             "              \\-HDD, 750 GiB\n"
             "                +-[0]: 100 GiB, root\n"
             "                \\-[1]: 600 GiB, log\n" );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
