package main

import (
	"fmt"
	"strconv"
	"strings"
)

/*sudo tcpdump -i any -X  -A port 9999

-i interface
    lo0 监听lo0
    any 所有接口(not support promiscuous，监听其它主机的接口 )
-n
    遇到协议号或端口号时，不要将这些号码转换成对应的协议名称或端口名称。比如，我们希望显示21，而非tcpdump自作聪明的将它显示成FTP。
-X, -x(小写显示hex，不显示ascii)
    把协议头和包内容都以16 进制显示出来（tcpdump会以16进制和ASCII的形式显示），这在进行协议分析时是绝对的利器。
-A
    选项，则tcpdump只会显示ASCII形式的数据包内容，不会再以十六进制形式显示；

-XX, -xx(小写显示hex，不显示ascii)
    选项，则tcpdump会从以太网部分就开始显示网络包内容，而不是仅从网络层协议开始显示。一般不需要这么低层
-c count
    只抓一个包
port 8000
    监听8000 端口
*/

/*
	写一个解析tcp抓包结果，并将其输出的小工具
	sudo tcpdump -i any -x  -A -l port 9999 | go run parse_tcp_packet.go
*/

func main(){
	type tcpPacket struct{
		SrcAddr string
		DstAddr string
		Flags	string
		seq		string
		ack		string
		options	string
		length	int
		win		int
	}
	type IPHex struct {
		Version int
		HeadLen int
		TOS string
		AllLen int
		Certification int
		MF int
		DF int
		Offset int
		TTL int
		ProtocolType string
		FirstCheckSum int
		SrcAddr string
		DstAddr string
	}
	for{
		var skip string//被定义接受
		/* time */
		var timeStamp string
		fmt.Scan(&timeStamp)
		fmt.Println("time is ",timeStamp)

		/*TCP Packets*/
		tcpPack:=tcpPacket{}
		for{
			var statement string
			var temp string
			flag:=false
			fmt.Scan(&statement)
			switch statement{
				case "IP","IP6":
					fmt.Scan(&tcpPack.SrcAddr,&temp,&tcpPack.DstAddr)
				case "Flags":
					fmt.Scan(&tcpPack.Flags)
				case "seq":
					fmt.Scan(&tcpPack.seq)
				case "win":
					fmt.Scan(&tcpPack.win)
				case "ack":
					fmt.Scan(&tcpPack.ack)
				case "options":
					for{
						fmt.Scan(&temp)
						tcpPack.options=tcpPack.options+temp+","
						if strings.Contains(temp,"]") {
							tcpPack.options=strings.TrimRight(tcpPack.options,",")
							break
						}
					}
				case "length":
					fmt.Scan(&tcpPack.length)
					break
				default:
					flag=true
					break
			}
			if flag{
				break
			}
		}
		fmt.Printf("%+v\n",tcpPack)
		/*Hex IP packets*/

		var Hex string
		for{
			fmt.Scan(&skip)
			/*4位版本+4位首部长度*/
			fmt.Scan(&Hex)
		}
	}

}

//func hexToInt(s string) int{
//	bs, _ := hex.DecodeString(s)
//	num := binary.BigEndian.Uint16(bs[:2])
//	return int(num)
//}
//
//func getHigh(a int) int{
//	return a&65280
//}
//
//func getLow(a int) int{
//	return a&255
//}

func hex2bin(hex string) string {
	var bin string

	for i := 0; i < len(hex); i++ {
		hex2int, _ := strconv.ParseInt(string(hex[i]), 16, 64)
		bin = bin + fmt.Sprintf("%04b", hex2int)
	}

	return bin
}