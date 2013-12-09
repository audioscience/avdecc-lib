/* Code generated from ./mkbpf in this dir */
static int ethernet_proto_bpf[1] = {
	0x22f0
};
/*
tcpdump -d  ether proto 0x22f0 or vlan 2
(000) ldh      [12]
(001) jeq      #0x22f0          jt 6	jf 2
(002) jeq      #0x8100          jt 3	jf 7
(003) ldh      [14]
(004) and      #0xfff
(005) jeq      #0x2             jt 6	jf 7
(006) ret      #65535
(007) ret      #0
tcpdump -dd  ether proto 0x22f0 or vlan 2
*/
struct sock_filter BPF_code[] = {
{ 0x28, 0, 0, 0x0000000c },
{ 0x15, 4, 0, 0x000022f0 },
{ 0x15, 0, 4, 0x00008100 },
{ 0x28, 0, 0, 0x0000000e },
{ 0x54, 0, 0, 0x00000fff },
{ 0x15, 0, 1, 0x00000002 },
{ 0x6, 0, 0, 0x0000ffff },
{ 0x6, 0, 0, 0x00000000 },
};
