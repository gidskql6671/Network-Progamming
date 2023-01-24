# Network Programming
네트워크 프로그래밍을 안 듣고 컴퓨터망을 듣는 호구를 위해...

## 이론 정리
### 소켓의 타입
연결지향형 소켓(`SOCK_STREAM`)  
- 중간에 데이터가 소멸되지 않고 목적지로 전송된다.
  - 독립된 연결을 통해 데이터를 전달하기에, 연결 자체가 문제가 있는게 아닌 이상 데이터가 소멸되지 않음을 보장한다.
- 전송 순서대로 데이터가 수신된다.  
- 전송되는 데이터의 경계(Boundary)가 존재하지 않는다.  
  - 데이터를 송수신하는 소켓은 내부적으로 버퍼를 가지고 있다. 이 버퍼를 사용하여 데이터를 나눠서 받을수도 혹은 한번에 받을 수도 있다. 그렇기에 데이터의 경계가 없다고 한다.
- 소켓 대 소켓의 연결은 반드시 1대1이어야 한다.
- 신뢰성 있는 순차적인 바이트 기반의 연결지향 데이터 전송 방식의 소켓
- TCP 프로토콜이 이에 해당한다.

비 연결지향형 소켓(`SOCK_DGRAM`)
- 전송된 순서에 상관없이 가장 빠른 전송을 지향한다.
- 전송된 데이터는 손실의 우려가 있고, 파손의 우려가 있다.
- 전송되는 데이터의 경계(Boundary)가 존재한다.
- 한번에 전송할 수 있는 데이터의 크기가 제한된다.
- 신뢰성과 순차적 데이터 전송을 보장하지 않는, 고속의 데이터 전송을 목적으로 하는 소켓
- UDP 프로토콜이 이에 해당한다.

### 인터넷 주소(Internet Address)
IP(Internet Protocol) 주소 체계는 IPv4와 IPv6 두 종류로 나뉜다. 두 체계의 차이는 IP주소 표현에 사용되는 바이트 크기이며, 자세한 설명은 생략.  

IPv4 기준의 4바이트 IP 주소는 네트워크 주소와 호스트(컴퓨터를 의미) 주소로 나뉘며, 주소의 형태에 따라 A, B, C, D, E 클래스로 분류된다.  

- A 클래스 : 앞 1바이트가 네트워크 주소, 뒤 3바이트가 호스트 주소
- B 클래스 : 앞 2바이트가 네트워크 주소, 뒤 2바이트가 호스트 주소
- C 클래스 : 앞 3바이트가 네트워크 주소, 뒤 1바이트가 호스트 주소
- D 클래스 : 멀티캐스트 IP 주소
- E 클래스 : 예약된 IP 주소

네트워크 주소와 호스트 주소로 나뉘어져 있는 이유는 IP 주소로 컴퓨터를 찾을 때, 한번에 바로 접근하는 방식이 아니기 때문이다. 네트워크 주소를 사용하여 특정 네트워크에 접근한 뒤에 호스트 주소를 사용하여 특정 호스트로 접근하는 방식을 사용한다. 예를 들어, SEMI.COM라는 회사의 무대리에게 데이터를 전송한다고 가정해보자. 그러면 우선 네트워크 주소를 사용하여 SEMI.COM의 네트워크로 데이터가 전송되고, 이후 해당 네트워크를 구성하는 라우터가 호스트 주소를 사용하여 무대리에게 데이터를 전송한다.  

> 송신자 ---(네트워크 주소)---> 네트워크(라우터, 스위치) ---(호스트 주소)---> 호스트  

위 흐름에서 알 수 있듯 특정 IP 주소에서 네트워크 주소와 호스트 주소를 아는 것은 중요하다. 이것은 IP 주소의 첫 번째 바이트를 보면 알 수 있다. 왜냐하면 다음과 같이 클래스 별로 IP 주소의 경계를 나눠놓았기 때문이다.

- 클래스 A의 첫 번째 바이트 범위 : 0이상 127이하 
- 클래스 B의 첫 번째 바이트 범위 : 128이상 191이하 
- 클래스 C의 첫 번째 바이트 범위 : 192이상 223이하 

이는 다음과 같이 표현할 수도 있다.

- 클래스 A의 첫 번째 비트는 항상 `0`으로 시작
- 클래스 B의 첫 번째 비트는 항상 `10`으로 시작
- 클래스 C의 첫 번째 비트는 항상 `110`으로 시작

### PORT 번호
IP 주소가 네트워크 상의 컴퓨터를 구분하기 위한 목적으로 사용된다면, PORT 번호는 컴퓨터 내에서 프로세스를 구분하기 위한 목적으로 사용된다.  

PORT 번호는 16비트로 표현되기에, 할당할 수 있는 PORT 번호의 범위는 0이상 65535이하이다. 하지만 0부터 1023까지는 '잘 알려진 PORT(Well-known PORT)'라서, 사용처가 이미 정해져있다. 추가로 PORT 번호는 기본적으로 중복이 불가능하지만, TCP 소켓과 UDP 소켓은 PORT 번호를 공유하지 않아 중복되어도 상관없다.  

### 바이트 순서(Order)와 네트워크 바이트 순서
CPU가 데이터를 저장하는/해석하는 방식은 두 가지로 나뉜다.  

- 빅 엔디안(Big Endian) : 상위 바이트의 값을 작은 번지수에 저장하는 방식
- 리틀 엔디안(Little Endian) : 상위 바이트의 값을 큰 번지수에 저장하는 방식  

예를 들어, 0x20번지를 시작으로 4바이트 Int형 정수 0x12345678을 저장한다고 가정하면, 다음과 같이 저장된다.  

| | 0x20번지 | 0x21번지 | 0x22번지 | 0x23번지
|-----|-----|-----|-----|-----|
| 빅 엔디안 | 0x12 | 0x34 | 0x56 | 0x78
| 리틀 엔디안 | 0x78 | 0x56 | 0x34 | 0x12

데이터의 저장 방식은 CPU에 따라 달라지며, 이러한 CPU의 데이터 저장방식을 '호스트 바이트 순서(Host Byte Order)'라고 한다. 만약 호스트 바이트 순서가 서로 다른 컴퓨터끼리 데이터를 그냥 주고받는다면, 0x1234라는 데이터를 보냈을 때 상대방은 0x3412라는 값으로 받아들일 것이다. 이런 문제를 해결하기 위해 네트워크를 통해 데이터를 전송할 때는 통일된 기준으로 데이터를 전송하기로 정하게 되었다. 이를 '네트워크 바이트 순서(Network Byte Order)'라고 하며, 빅 엔디안 방식으로 통일하기로 했다. 

즉, 네트워크를 통해 수신된 데이터는 빅 엔디안 방식으로 정렬되어 있음이 보장되고, 리틀 엔디안 시스템에서 데이터를 전송할 때는 빅 엔디안 방식으로 데이터를 재정렬할 필요가 있다.  

C언어의 소켓 프로그래밍에서는 네트워크 바이트 순서로 데이터를 재정렬하는 함수를 제공한다. `htons`, `ntohs`, `htonl`, `ntohl`이 그것이다. 이 함수들에서 `h`는 호스트 바이트 순서를 의미하고, `n`은 네트워크 바이트 순서를 의미한다. `s`는 short, `l`은 long(4바이트)을 의미한다. 즉, `htons`는 "short형 데이터를 호스트 바이트 순서에서 네트워크 바이트 순서로 변환하는 함수"이다. 일반적으로 뒤에 `s`가 붙으면 PORT 번호의 변환을, `l`이 붙으면 IP주소의 변환에 사용한다. (타입이 같으니)  

추가적으로 이러한 변환은 소켓 프로그래밍에서 `sockaddr_in` 변수에 개발자가 직접 값을 채워줄 때만 해주면 된다. 데이터 송수신 과정에서는 함수 내부에서 알아서 데이터 재정렬을 해준다.  

