pragma solidity >=0.4.16 <0.7.0;

contract Paylock {
    
    enum State { Working , Completed , Done_1 , Delay , Done_2 , Forfeit }
    
    int disc;
    State st;
    int clock;
    int collect_1_N_time;
    address timeAdd;
    
    constructor(address _timeAdd) public {
        st = State.Working;
        disc = 0;
        clock = 0;
        collect_1_N_time = -1;
        timeAdd = _timeAdd;
    }

    function tick() public {
        require(msg.sender == timeAdd);
        clock += 1;
    }

    function signal() public {
        require( st == State.Working );
        st = State.Completed;
        disc = 10;
        
    }

    function collect_1_Y() public {
        require( st == State.Completed );
        require(clock < 4);
        st = State.Done_1;
        disc = 10;
    }

    function collect_1_N() external {
        require( st == State.Completed );
        require(clock >= 4);
        st = State.Delay;
        disc = 5;
        collect_1_N_time = clock;
    }

    function collect_2_Y() external {
        require( st == State.Delay );
        require(collect_1_N_time >= 0 && clock < collect_1_N_time + 4);
        st = State.Done_2;
        disc = 5;
    }

    function collect_2_N() external {
        require( st == State.Delay );
        require(collect_1_N_time >= 0 && clock >= collect_1_N_time + 4);
        st = State.Forfeit;
        disc = 0;
    }

}

contract Supplier {
    
    Paylock p;
    Rental rental;
    bool hasResource;
    
    enum State { Working , Completed }
    
    State st;

    uint reentrancyCount;
    
    constructor(address pp, address payable _rental) public {
        p = Paylock(pp);
        rental = Rental(_rental);
        st = State.Working;
        hasResource = false;
        reentrancyCount = 0;
    }

    receive() external payable {
        if (reentrancyCount < 5) {
            reentrancyCount++;
            (bool success, ) = address(rental).call(abi.encodeWithSignature("retrieve_resource()"));
            require(success);
        }
    }

    function aquire_resource() external payable {
        require(st == State.Working);
        require(!hasResource);
        (bool success, ) = address(rental).call.value(1 wei)(abi.encodeWithSignature("rent_out_resource()"));
        require(success);
        hasResource = true;
    }

    function return_resource() external {
        require(st == State.Working);
        require(hasResource);
        reentrancyCount = 0;
        (bool success, ) = address(rental).call(abi.encodeWithSignature("retrieve_resource()"));
        require(success);
        hasResource = false;
    }
    
    function finish() external {
        require (st == State.Working);
        require(!hasResource);
        p.signal();
        st = State.Completed;
    }
    
    function getBalance() public view returns (uint) {
        return address(this).balance;
    }
}

contract Rental {
    
    address payable resource_owner;
    bool resource_available;
    
    constructor() public payable {
        resource_available = true;
    }

    receive() external payable {}
    
    function rent_out_resource() external payable {
        require(resource_available == true);
        require(msg.value == 1 wei);
        resource_owner = msg.sender;
        resource_available = false;
    }

    function retrieve_resource() external {
        require(resource_available == false && msg.sender == resource_owner);
        (bool success, ) = resource_owner.call.value(1 wei)("");
        require(success);
        resource_available = true;
    }

    function getBalance() public view returns (uint) {
        return address(this).balance;
    }
}