# PA2-fourth-project
The task is to design and implement a set of classes that will simulate the registration of the company's computer equipment. Specifically, we will store information about networks (CNetwork), computers (CComputer), their processors (CCPU), memories (CMemory) and disks (CDisk).


The task is focused on the design of classes where inheritance, polymorphism and abstract methods will be used. If these OOP resources are used rationally, the implementation is not too long. On the contrary, if you implement the design incorrectly, the code will repeat itself and the implementation file will be large. Try to identify a base class and derive subclasses from it appropriately by inheritance.

Classes and their interfaces:

CNetwork
    represents the network. Its interface must include:
    constructor with the name of the network,
    destructor, copy constructor and = operator (if custom implementation is needed),
    addComputer method to add another computer to the network,
    findComputer method, which returns a reference to the found computer of the specified name, or an invalid reference if it does not find one. Attention, it is also searched in virtual computers that, for example, run inside another computer that is connected to this      network,
    findNetwork method, which returns a link to the found network of the given name, or an invalid link if it doesn't find one. Attention, it is also searched in virtual networks that, for example, run inside another computer that is connected to this network,
    output operator that displays a tree of computers and components, as in the example. Computers are listed in order of addition.
CComputer
    represents the computer. Its interface must include:
    constructor with a computer name parameter
    destructor, copy constructor and = operator (if custom implementation is needed),
    the addComponent method, which adds another computer component,
    addAddress method, which adds another computer address (string),
    findComputer method, which returns a reference to the found computer of the specified name, or an invalid reference if it does not find one. The result can be this computer itself or even a virtual machine running on this computer,
    findNetwork method, which returns a link to the found network of the given name, or an invalid link if it doesn't find one. Attention, it is also searched in virtual networks that, for example, run inside this computer,
    duplicate(remap) method, which creates a copy of this computer. An identical copy is created (except for any name changes), including any virtualized computers/networks. In addition, when creating a copy, the method changes the names of all copied computers and          networks according to the (remap) parameter. This parameter consists of pairs of strings (original name, new name). If the copied computer/network name appears in the remap parameter as some original name, it will be replaced. If the original name is not included in     the parameter, it will be left unchanged.
    operator for an output that displays the assigned addresses and computer components, as in the example. In the list, the addresses are listed first (in the order of entry) followed by the components (in the order of addition).
CCPU
    represents the CPU. Its interface must include:
    constructor with parameter number of cores (integer) and frequency (integer in MHz),
    a destructor, a copy constructor, and the = operator (if a custom implementation is needed).
CMemory
    represents RAM. Its interface must include:
    constructor with memory size parameter (integer in MiB),
    a destructor, a copy constructor, and the = operator (if a custom implementation is needed).
CDDisc
    represents storage. Its interface must include:
    constructor with disk type parameters (SSD or MAGNETIC symbolic constant declared in the class) and disk size (integer in GiB),
    destructor, copy constructor and = operator (if custom implementation is needed),
    the addPartition method, which adds information about disk partitioning. The method will have two parameters - the size of the parcel in GiB and its identification (string). The list of parcels is in order of entry.
