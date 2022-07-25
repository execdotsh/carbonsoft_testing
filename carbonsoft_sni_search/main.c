#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include "skbuff.h"
#include "sni.h"
#include "packets/client_hello.h"
#include "packets/client_hello_tcp32.h"
#include "packets/client_hello_ambigous_doff.h"
#include "packets/not_client_hello.h"

// функция поиска ssl hello по сигнатуре
bool try_find_ssl_hello(unsigned char *begin, unsigned char *end)
{
	struct {
		// record
		unsigned char content_type; // 0x16 -> handshake
		unsigned char version_major;
		unsigned char version_minor;
		unsigned char content_len[2];
		// hanshake
		unsigned char handshake_type; // 0x01 -> Client Hello
		unsigned char handshake_len[3];
	} hello_signature = { 0 };

	while (true) {
		begin = memchr(begin, '\x16', end - begin);
		if (begin == NULL || end - begin < sizeof(hello_signature))
			return false;
		memcpy(&hello_signature, begin, sizeof(hello_signature));
		if (
			hello_signature.version_major == 0x03 
			&& hello_signature.handshake_type == 0x01
			&& hello_signature.handshake_len[0] == 0x00)
		{
			unsigned content_len =
			    hello_signature.content_len[0] * 256 +
			    hello_signature.content_len[1];
			unsigned handshake_len =
			    hello_signature.handshake_len[1] * 256 +
			    hello_signature.handshake_len[2];
			if (content_len == handshake_len + 4)
				return true;
		}
		begin += 1;
	}
}

/*
Нельзя просто взять и искать SSL_CONTENT_TYPE_HANDSHAKE (0x16) в пакете,
он там окажется с вероятностью 256/длина_пакета.
SSL_CONTENT_TYPE должен располагаться строго после TCP-заголовка.
Но где он заканчивается, если пакеты приходят с полем tcp->doff = 0?
*/

bool snimatch_mt(unsigned char *pkt, size_t len)
{
	struct sk_buff *skb = read_skb(pkt, len);

	// получаем TCP хэдр перемещая указатель на длину заголовка IP через
	// поле Internet Header Length
	unsigned char ip_length = (*skb->data & 0x0F) * 4;
	if (ip_length < 20 || ip_length > 60)
		return false;
	unsigned char *tcp_header = skb->data + ip_length;

	// получаем размер заголовка TCP через поле Data Offset
	const int doff_offset = 0x0C;
	unsigned char tcp_doff = (*(tcp_header + doff_offset) >> 4) * 4;
	// если в поле Data Offset неверное значение то попробуем найти ssl hello по сигнатуре
	if (tcp_doff < 20 || tcp_doff > 60)
		return try_find_ssl_hello(tcp_header + 20, skb->raw + skb->len);

	return verify_ssl(tcp_header + tcp_doff, skb->raw + skb->len) != NULL;
}

int main()
{
	printf("# client_hello.h: ");
	if (snimatch_mt(client_hello, sizeof(client_hello)))
		printf(" OK\n");
	else
		return 1;

	printf("# client_hello_tcp32.h");
	if (snimatch_mt(client_hello_tcp32, sizeof(client_hello_tcp32)))
		printf(" OK\n");
	else
		return 1;

	printf("# not_client_hello.h");
	if (!snimatch_mt(not_client_hello, sizeof(not_client_hello)))
		printf(" OK\n");
	else
		return 1;

	printf("# client_hello_ambigous_doff.h");
	if (snimatch_mt(client_hello_ambigous_doff, sizeof(client_hello_ambigous_doff)))
		printf(" OK\n");
	else
		return 1;
	return 0;
}
