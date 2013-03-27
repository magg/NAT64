#ifndef _NF_NAT64_IPV6_POOL_H
#define _NF_NAT64_IPV6_POOL_H

/**
 * @file
 * The pool of IPv6 addresses.
 *
 * @author Alberto Leiva
 */

#include <linux/types.h>
#include <linux/in6.h>
#include "nat64/comm/types.h"
#include "nat64/comm/config_proto.h"


bool pool6_init(void);
void pool6_destroy(void);

int pool6_register(struct ipv6_prefix *prefix);
int pool6_remove(struct ipv6_prefix *prefix);

bool pool6_contains(struct in6_addr *address);
bool pool6_peek(struct ipv6_prefix *out);
int pool6_for_each(int (*func)(struct ipv6_prefix *, void *), void * arg);

#endif /* _NF_NAT64_IPV6_POOL_H */
