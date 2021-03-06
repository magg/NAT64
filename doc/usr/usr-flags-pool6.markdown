---
layout: documentation
title: Documentation - Userspace Application
---

# [Doc](doc-index.html) > [Userspace App](doc-index.html#userspace-application) > [Flags](usr-flags.html) > \--pool6

## Index

1. [Description](#description)
2. [Syntax](#syntax)
3. [Options](#options)
   1. [Operations](#operations)
   2. [\--quick](#quick)
   3. [\--prefix](#prefix)
4. [Examples](#examples)

## Description

Interacts with Jool's IPv6 pool. The pool dictates which packets coming from the IPv6 side are processed; if an incoming packet's destination address has one of the IPv6 prefixes, the packet is translated. Otherwise it is handed to the kernel to either be forwarded in some other way or handed to the upper layers.

## Syntax

	jool --pool6 [--display]
	jool --pool6 --count
	jool --pool6 --add --prefix <prefix>
	jool --pool6 --remove --prefix <prefix> [--quick]
	jool --pool6 --flush [--quick]

## Options

### Operations

* `--display`: The pool's prefixes are printed in standard output. This is the default operation.
* `--count`: The number of prefixes in the pool is printed in standard output.
* `--add`: Uploads `<prefix>` to the pool.
* `--remove`: Deletes from the tables the prefix `<prefix>`.
* `--flush`: Removes all prefixes from the pool.

### \--quick

See [`--quick`](usr-flags-quick.html).

### \--prefix

	<prefix> := <prefix address>/<prefix length>

## Examples

{% highlight bash %}
$ # Display the current prefixes.
$ jool --pool6
64:ff9b::/96
  (Fetched 1 prefixes.)
$ # Display only the prefix count.
$ jool --pool6 --count
1
$ # Remove the default prefix.
$ jool --pool6 --remove --prefix 64:ff9b::/96
$ # Return the default prefix.
$ jool --pool6 --add --prefix 64:ff9b::/96
$ # Destroy all prefixes. Do not bother cleaning up the garbage.
$ jool --pool6 --flush --quick
{% endhighlight %}

