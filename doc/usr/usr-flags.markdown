---
layout: documentation
title: Documentation - Userspace Application
---

# Userspace Application > Flags

## Index

1. [\--help](#help)
2. [\--pool6](#pool6)
3. [\--pool4](#pool4)
4. [\--bib](#bib)
5. [\--session](#session)
6. [\--quick](#quick)
7. [\--general](#general)
   1. [\--dropAddr](#dropaddr)
   2. [\--dropInfo](#dropinfo)
   3. [\--dropTCP](#droptcp)
   4. [\--toUDP](#toudp)
   5. [\--toTCPest](#totcpest)
   6. [\--toTCPtrans](#totcptrans)
   7. [\--toICMP](#toicmp)
   8. [\--maxStoredPkts](#maxstoredpkts)
   9. [\--setTC](#settc)
   10. [\--setTOS](#settos)
   11. [\--TOS](#tos)
   12. [\--setDF](#setdf)
   13. [\--genID](#genid)
   14. [\--boostMTU](#boostmtu)
   15. [\--plateaus](#plateaus)
   16. [\--minMTU6](#minmtu6)

## Introduction

This document explains the flags and options of Jool's userspace application.

See the [compilation and installation](usr-install.html) instructions if you still don't have the binaries.

If a command changes the behavior of Jool, it requires network admin privileges (<a href="http://linux.die.net/man/7/capabilities" target="_blank">CAP_NET_ADMIN</a>).

## \--help

**Syntax**

	jool --help

**Description**

Prints mostly a summary of this document, though you can also use it to review the abbreviated form of the flags, which aren't here.

`--help` is the only mode which does not require Jool's kernel module to be active.

## \--pool6

**Syntax**

	jool --pool6 [--display]
	jool --pool6 --count
	jool --pool6 --add --prefix <prefix>
	jool --pool6 --remove --prefix <prefix> [--quick]
	jool --pool6 --flush [--quick]

**Arguments**

	<prefix> := <prefix address>/<prefix length>

**Description**

Interacts with Jool's IPv6 pool. The pool dictates which packets coming from the IPv6 side are processed; if an incoming packet's destination address has one of the IPv6 prefixes, the packet is translated. Otherwise it is handed to the kernel to either be forwarded in some other way or handed to the upper layers.

**Operations**

* Using `--display`, the application prints Jool's current prefixes. This is the default operation.
* Using `--count`, Jool prints the number of prefixes in the database.
* Using `--add`, Jool adds the `<prefix>` prefix to the pool.
* Using `--remove`, Jool deletes the `<prefix>` prefix from the pool.
* Using `--flush`, Jool empties the pool.

**Additional flags**

See [`--quick`](#quick).

**Examples**

{% highlight bash %}
# Display the current prefixes.
jool --pool6
# Display only the prefix count.
jool --pool6 --count
# Remove the default prefix.
jool --pool6 --remove --prefix 64:ff9b::/96
# Return the default prefix.
jool --pool6 --add --prefix 64:ff9b::/96
# Destroy all prefixes. Do not bother cleaning up the garbage.
jool --pool6 --flush --quick
{% endhighlight %}

## \--pool4

**Syntax**

	jool --pool4 [--display]
	jool --pool4 --count
	jool --pool4 --add --address <IPv4 address>
	jool --pool4 --remove --address <IPv4 address> [--quick]
	jool --pool4 --flush [--quick]

**Description**

Interacts with Jool's IPv4 pool. The pool dictates which packets coming from the IPv4 side are processed; if an incoming packet's destination address is listed in the pool, the packet is translated. Otherwise it is handed to the kernel to either be forwarded in some other way or handed to the upper layers.

**Operations**

* Using `--display`, the application prints Jool's current addresses. This is the default operation.
* Using `--count`, Jool prints the number of addresses in the pool.
* Using `--add`, Jool adds `<IPv4 address>` to the pool.
* Using `--remove`, Jool deletes `<IPv4 address>` from the pool.
* Using `--flush`, Jool empties the pool.

**Additional flags**

See [`--quick`](#quick).

**Examples**

{% highlight bash %}
# Display the current addresses.
jool --pool4
# Display only the address count.
jool --pool4 --count
# Remove one of default addresses.
jool --pool4 --remove --address 192.168.2.2
# Return the address.
jool --pool4 --add --address 192.168.2.2
{% endhighlight %}

## \--bib

**Syntax**

	jool --bib [--display] [--numeric] <protocols>
	jool --bib --count <protocols>
	jool --bib --add <protocols> --bib4 <bib4> --bib6 <bib6>
	jool --bib --remove <protocols> --bib4 <bib4> --bib6 <bib6>
	jool --bib --flush <protocols>

**Arguments**

	<protocols> := [--tcp] [--udp] [--icmp]
	<bib4> := <IPv4 address>#(<port> | <ICMP identifier>)
	<bib6> := <IPv6 address>#(<port> | <ICMP identifier>)

**Description**

Interacts with Jool's Binding Information Base (BIB). Records in this database map IPv6 transport addresses (of the IPv6 nodes the NAT64 is servicing) with their current IPv4 transport address (from the NAT64's pool). For example, if you see the following output:

{% highlight bash %}
$ jool --bib --tcp --numeric
TCP:
[Dynamic] 4.4.4.4#44 - 6::6#66
  (Fetched 1 entries.)
{% endhighlight %}

Then some node from the IPv4 side is chatting 6::6 on port 66 using the TCP protocol. Jool is fooling the IPv4 node into thinking that 6::6#66's address is 4.4.4.4 on port 44. If you want more details on the relationship (such as the IPv4 node's address), head over to [\--session](#session).

The BIB is a very fundamental component of stateful NAT64. If you want Jool to be able to translate a packet stream, then you need a corresponding record in the BIB.  
When translating from IPv6 to IPv4, Jool has enough information to create bindings on its own. As such, you do not need to create manual bindings for traffic IPv6 nodes are going to start.  
Jool cannot create entries when traffic is started from IPv4 because there's not enough information to create the record. If IPv4 nodes are supposed to be able to start communication (e.g. you want to publish an IPv6 node's service to the IPv4 world) then you need to create manual bindings ([here's the walkthrough](op-static-bindings.html)).

(When I say "start communication" I mean the first packet in a stream. If the IPv4 node is going to answer a request emitted by an IPv6 node, it counts as a packet from the same stream. Hence you don't need static BIB entries for these.)

Jool automatically kills dynamic entries once their sessions expire; Jool never automatically kills static entries. Both kind of entries can be deleted manually.

The database consists of three separate tables; one for TCP bindings, one for UDP bindings, and one for ICMP bindings. As a result, if you add an entry (e.g. `66::66#6 <-> 44.44.44.44#4`) manually to one of the tables (e.g. TCP), then Jool will not attempt to translate a first IPv4 request to the same address#port and different protocol (e.g. 44.44.44.44#4 on UDP).

**Operations**

* Using `--display`, the application prints Jool's current BIB. This is the default operation.
* Using `--count`, Jool prints the number of BIB entries per table.
* Using `--add`, Jool adds the entry resulting from the `--bib6` and `--bib4` parameters to the BIB. Note that the `--bib4` component is an address assigned to the NAT64, so make sure you have added it to the [IPv4 pool](#pool4).
* Using `--remove`, Jool deletes the entry. Since both components are unique across all entries from the same table, you only need to supply one of the `--bib*` arguments (but you can still input both to make sure you're deleting exactly what you want to delete).

**Protocols**

The command will only affect the tables mentioned by these parameters. If you omit this entirely, Jool will assume you want to operate on all three tables.

**Additional flags**

When `--display`ing, the application will by default attempt to resolve the name of the IPv6 service of each BIB entry. _If your nameservers aren't answering, this will slow the output down_.

{% highlight bash %}
$ jool --bib --tcp
TCP:
[Dynamic] 4.4.4.4#44 - ipv6service.mx#66
  (Fetched 1 entries.)
{% endhighlight %}

Use `--numeric` to turn this behavior off (see the example in the description section, above).

**Examples**

{% highlight bash %}
# Display only the TCP and UDP tables.
jool --bib --display --tcp --udp
# Same, but do not query the DNS.
jool --bib --display --tcp --udp --numeric
# Display the number of entries in the TCP and ICMP tables.
jool --bib --count --tcp --icmp
# Add an entry to the UDP table.
jool --bib --add --udp --bib6 6::6#66 --bib4 4.4.4.4#44
# Remove the entry we just added.
jool --bib --remove --udp --bib6 6::6#66
{% endhighlight %}

You might ask:

_Since the session tables contain all the information from the Binding Information Bases (BIB) and more, what's the point of the BIBs?_

From the user's perspective, the difference between BIB entries and session entries is meaningful because each of them is queried for their particular reasons:

- You want to query the BIBs when you want to know the IPv4 name the NAT64 has assigned to each of your IPv6 nodes.
- You want to query the session tables when you want to know who exactly is chatting your IPv6 nodes.

That is by far the most important distinction.

Another one is that if we merged the BIB entries into the session entries, the implementation of static BIB entries would be pretty awkward because they often don't have sessions associated with them.

## \--session

**Syntax**

	jool --session [--display] [--numeric] <protocols>
	jool --session --count <protocols>

**Arguments**

	<protocols> := [--tcp] [--udp] [--icmp]

**Description**

Sessions mostly exist so the NAT64 can decide when BIB entries should die. You can also use them to know exactly who is speaking to your IPv6 nodes. In a future release you will also be able to <a href="https://github.com/NICMx/NAT64/issues/41" target="_blank">define filtering policies based on them</a>.

Each BIB entry is a mapping, which describes the IPv4 name of one of your IPv6 services. For every BIB entry, there are zero or more session entries, each of which represents an active connection currently using that mapping.

You can use this command to get information on each of these connections.

**Operations**

* Using `--display`, the application prints Jool's current sessions. This is the default operation.
* Using `--count`, Jool prints the number of sessions per table.

**Protocols**

The command will filter out the tables not mentioned in this list. If you don't include any protocols though, Jool will assume you want to see all three tables.

**Other parameters**

When `--display`ing, the application will attempt to resolve the names of the remote nodes talking in each session. _If your nameservers aren't answering, this will slow the output down_. Use `--numeric` to turn this behavior off.

**Command examples**

{% highlight bash %}
# Display only the ICMP table. Do not query the DNS.
jool --session --display --icmp --numeric
# Same, shorter version.
jool --session --icmp --numeric
# Same, shorter+ version.
jool -sin
# We don't need every entry, just print the size of the database.
jool --session --count
{% endhighlight %}

**Sample outputs**

![Fig.1 - Session sample network](images/usr-session.svg)

ipv6client.mx makes two HTTP requests to example.com:

{% highlight bash %}
$ jool --session --display --tcp --numeric
TCP:
---------------------------------
Expires in 3 minutes, 24 seconds
Remote: 93.184.216.119#80	2001:db8::2#58239
Local: 192.0.2.1#60477		64:ff9b::5db8:d877#80
---------------------------------
Expires in 3 minutes, 24 seconds
Remote: 93.184.216.119#80	2001:db8::2#58237
Local: 192.0.2.1#6617		64:ff9b::5db8:d877#80
---------------------------------
  (Fetched 2 entries.)
{% endhighlight %}

{% highlight bash %}
$ jool --session --display --tcp
TCP:
---------------------------------
Expires in 3 minutes, 24 seconds
Remote: example.com#http	ipv6client.mx#58239
Local: 192.0.2.1#60477		64:ff9b::5db8:d877#80
---------------------------------
Expires in 3 minutes, 24 seconds
Remote: example.com#http	ipv6client.mx#58237
Local: 192.0.2.1#6617		64:ff9b::5db8:d877#80
---------------------------------
  (Fetched 2 entries.)
{% endhighlight %}

## \--quick

First, a little background information:

* [IPv6 prefix](#pool6) _P_ owns [session entry](#session) _S_ if _P_ equals the network side of _S_'s local IPv6 address.
* [IPv4 address](#pool4) _A_ owns [BIB entry](#bib) _B_ if _A_ equals _B_'s IPv4 address.

If you `--remove` or `--flush` an owner, its "slaves" become obsolete because the relevant packets are no longer going to be translated.

* If you omit `--quick` while removing owners, Jool will get rid of the newly orphaned slaves. This saves memory and keeps entry lookup efficient during packet translations.
* On the other hand, when you do issue `--quick`, Jool will only purge the owners.  You might want to do this if you want the operation to succeed quickly (maybe you have a HUGE amount of slaves), or more likely you plan to re-add the owner in the future (in which case the still-remaining slaves will become relevant and usable again).

Orphaned slaves will remain inactive in the database, and will eventually kill themselves once their normal removal conditions are met (eg. orphaned sessions will die once their timeout expires).

## \--general

**Syntax**

	jool [--general]
	jool [--general] <flag key> <new value>

**Description**

Controls several of Jool's internal variables.

* Issue an empty `--general` command to display the current values of all of Jool's options.
* Enter a key and a value to edit the key's variable.

`--general` is the default configuration mode, so you never actually need to input that one flag.

**Examples**

{% highlight bash %}
# Display the configuration values, keys and values.
jool --general
# Same thing, shorter version.
# BTW: This looks very simple, but it still requires Jool's kernel module to be active.
jool
# Turn "address dependent filtering" on.
# true, false, 1, 0, yes, no, on and off all count as valid booleans.
jool --general --dropAddr true
{% endhighlight %}

**Keys**

The following flag keys are available:

### \--dropAddr

- Name: Address-dependent filtering
- Type: Boolean
- Default: OFF

If this is OFF, then IPv4 nodes might be able to initiate sessions to IPv6 nodes if Jool already has mappings involving those particular IPv6 nodes.

Turn `--dropAddr` ON to wreck any attempts of IPv4 nodes to initiate communication to IPv6 nodes.

Of course, this will not block IPv4 traffic if some IPv6 node first requested it.

This applies to UDP, TCP and ICMP info (ping) packets. ICMP errors are not affected by this rule.

### \--dropInfo

- Name: Filtering of ICMPv6 info messages
- Type: Boolean
- Default: OFF

If you turn this on, pings (both requests and responses) will be blocked while being translated from ICMPv6 to ICMPv4.

For some reason, we're not supposed to block pings from ICMPv4 to ICMPv6, but since you need both a request and a response for a successful echo, the outcome seems to be the same.

This rule will not affect Error ICMP messages.

### \--dropTCP

- Name: Dropping externally initiated TCP connections
- Type: Boolean
- Default: OFF

Similar to `--dropAddr`, except it only affects TCP packets.

### \--toUDP

- Name: UDP session lifetime
- Type: Integer (seconds)
- Default: 5 minutes

When a UDP session has been lying around inactive for this long, its entry will be removed from the database automatically.

When you change this value, the lifetimes of all already existing UDP sessions are updated.

### \--toTCPest

- Name: TCP established session lifetime
- Type: Integer (seconds)
- Default: 2 hours

When an established TCP connection has remained inactive for this long, its existence will be questioned. Jool will send a probe packet to one of the endpoints and kill the session if a response is not received before the `--toTCPtrans` timeout.

When you change this value, the lifetimes of all already existing established TCP sessions are updated.

### \--toTCPtrans

- Name: TCP transitory session lifetime
- Type: Integer (seconds)
- Default: 4 minutes

When a unhealthy TCP session has been lying around inactive for this long, its entry will be removed from the database automatically. A "unhealthy" session is one in which the TCP handshake has not yet been completed, it is being terminated by the endpoints, or is technically established but has remained inactive for `--toTCPest` time.

When you change this value, the lifetimes of all already existing transitory TCP sessions are updated.

### \--toICMP

- Name: ICMP session lifetime
- Type: Integer (seconds)
- Default: 1 minute

When a ICMP session has been lying around inactive for this long, its entry will be removed from the database automatically.

When you change this value, the lifetimes of all already existing ICMP sessions are updated.

### \--maxStoredPkts

- Name: Maximum number of stored packets
- Type: Integer
- Default: 10

When an external (IPv4) node first attempts to open a connection and there's no [BIB](#bib) entry for it, Jool normally answers with an Address Unreachable (type 3, code 1) ICMP error message, since it cannot know which IPv6 node the packet is heading.

In the case of TCP, the situation is a little more complicated because the IPv4 node might be attempting a <a href="https://github.com/NICMx/NAT64/issues/58#issuecomment-43537094" target="_blank">Simultaneous Open of TCP Connections</a>. To really know what's going on, Jool has to store the packet for 6 seconds.

`--maxStoredPkts` is the maximum amount of packets Jool will store at a time. The default means that you can have up to 10 "simultaneous" simultaneous opens; Jool will fall back to answer the ICMP error message on the eleventh one.

### \--setTC

- Name: Override IPv6 traffic class
- Type: Boolean
- Default: OFF
- Translation direction: IPv4 to IPv6

The <a href="http://en.wikipedia.org/wiki/IPv6_packet#Fixed_header" target="_blank">IPv6 header</a>'s Traffic Class field is very similar to <a href="http://en.wikipedia.org/wiki/IPv4#Header" target="_blank">IPv4</a>'s Type of Service (TOS).

If you leave this OFF, the TOS value will be copied directly to the Traffic Class field. If you turn this ON, Jool will always set Traffic Class as **zero** instead.

### \--setTOS

- Name: Override IPv4 type of service
- Type: Boolean
- Default: OFF
- Translation direction: IPv6 to IPv4

The <a href="http://en.wikipedia.org/wiki/IPv6_packet#Fixed_header" target="_blank">IPv6 header</a>'s Traffic Class field is very similar to <a href="http://en.wikipedia.org/wiki/IPv4#Header" target="_blank">IPv4</a>'s Type of Service (TOS).

If you leave this OFF, the Traffic Class value will be copied directly to the TOS field during IPv6-to-IPv4 translations. If you turn this ON, Jool will always set TOS as **\--TOS** instead.

### \--TOS

- Name: IPv4 type of service
- Type: Integer
- Default: 0
- Translation direction: IPv6 to IPv4

Value to set the TOS value of the packets' IPv4 fields during IPv6-to-IPv4 translations. _This only applies when **--setTOS** is ON; see above_.

### \--setDF

- Name: DF flag always on
- Type: Boolean
- Default: ON
- Translation direction: IPv6 to IPv4

If this is ON, Jool will always set the Don't Fragment flags (as 1).

If this is OFF, then Jool will set the DF flag only if the length of the IPv6 packet being translated is less or equal than 88 or higher than 1280 bytes. You do this to avoid black holes caused by ICMPv4 filtering or non-RFC2460-compatible IPv6 hosts.

### \--genID

- Name: Generate IPv4 identification
- Type: Boolean
- Default: OFF
- Translation direction: IPv6 to IPv4

All IPv4 packets contain an Identification field. IPv6 packets only contain an Identification field if they have a Fragment header.

If the incoming IPv6 packet has a fragment header, the <a href="http://en.wikipedia.org/wiki/IPv4#Header" target="_blank">IPv4 header</a>'s Identification field is _always_ copied from the low-order bits of the IPv6 fragment header's Identification value.

Otherwise:

- If `--genID` is OFF, the IPv4 header's Identification fields are set to zero.
- If `--genID` is ON, the IPv4 headers' Identification fields are set randomly.

### \--boostMTU

- Name: Decrease MTU failure rate
- Type: Boolean
- Default: ON
- Translation direction: IPv4 to IPv6 (ICMP errors only)

When a packet is too big for a link's MTU, routers generate <a href="http://tools.ietf.org/html/rfc4443#section-3.2" target="_blank">Packet too Big</a> ICMP errors on IPv6 and <a href="http://tools.ietf.org/html/rfc792" target="_blank">Fragmentation Needed</a> ICMP errors on IPv4. These error types are roughly equivalent, so Jool translates _Packet too Bigs_ into _Fragmentation Neededs_ and vice-versa.

These ICMP errors are supposed to contain the offending MTU so the emitter can resize and resend its packets accordingly.

The minimum MTU for IPv6 is 1280. The minimum MTU for IPv4 is 68. Therefore, Jool can find itself wanting to report an illegal MTU while translating a _Packet too Big_ into a _Fragmentation Needed_.

- If `--boostMTU` is OFF, Jool will not attempt to fix MTU values of _Packet too Big_ ICMP errors when they are too small.
- If `--boostMTU` is ON and an incoming _Fragmentation Needed_ reports a MTU which is smaller than 1280, Jool will report a MTU of 1280.

See <a href="http://tools.ietf.org/html/rfc6145#section-6" target="_blank">section 6 of RFC 6145</a> if you need the rationale.

### \--plateaus

- Name: MTU plateaus
- Type: List of Integers separated by commas (If you want whitespace, remember to quote).
- Default: "65535, 32000, 17914, 8166, 4352, 2002, 1492, 1006, 508, 296, 68"
- Translation direction: IPv4 to IPv6 (ICMP errors only)

See `--boostMTU` for a discussion on _Packet too Big_ and _Fragmentation Needed_.

The original ICMPv4 specification does not require IPv4 routers to report offending MTUs while responding _Fragmentation Needed_ errors, which means that some old hardware is bound to not set the field, which means that emitters will see zero MTUs.

In these cases, Jool will report as MTU the greatest plateau which is lower or equal than the incoming packet's Total Length field. Admittedly, this might or might not be the correct MTU, but is a very educated guess (See section 5 of <a href="http://tools.ietf.org/html/rfc1191" target="_blank">RFC 1191</a> for the rationale).

Note that if `--boostMTU` is activated, the MTU will still be 1280 if the resulting plateau is less than 1280.

Also, you don't really need to sort the values while you input them. Just saying.

### \--minMTU6

- Name: Minimum IPv6 MTU
- Type: Integer
- Default: 1280
- Translation direction: IPv4 to IPv6

All of your IPv6 networks have MTUs. You should set `--minMTU6` as the smallest of them.

IPv4 routers fragment, IPv6 routers don't fragment. If Jool receives a fragmentable IPv4 packet (Don't Fragment (DF) bit off), it has to make sure it's small enough to fit into any forthcoming IPv6 links (because the translation to IPv6 turns fragmentable packets into non-fragmentable packets). Otherwise, the smaller IPv6 hop will not let the packet through.

The way Jool "makes sure it's small enough" is by fragmenting the packet by itself. So, if a fragmentable IPv4 packet gets translated into a IPv6 packet whose length is higher than `--minMTU6`, Jool will fragment it prior to sending it.

So again, you want `--minMTU6` to be the smallest of your IPv6 MTUs so any of these formerly fragmentable packets will manage to fit into any IPv6 networks.

This value defaults to 1280 because all IPv6 networks are theoretically guaranteed to support at least 1280 bytes per packet. If all of your IPv6 networks have a higher MTU, you can raise `--minMTU6` to decrease chances of fragmentation.

- The penalty of `--minMTU6` being too small is performance; you get some unwanted fragmentation.
- The penalty of `--minMTU6` being too big is reliability; the IPv6 nodes which are behind networks with lesser MTUs will not be able to receive packets from IPv4 whose DF flag os off and which, once translated, are larger than `--minMTU6`.

IPv6 packets and unfragmentable IPv4 packets don't need any of this because they imply the emitter is the one minding MTUs and packet sizes (via <a href="http://en.wikipedia.org/wiki/Path_MTU_Discovery" target="_blank">Path MTU Discovery</a> or whatever).
