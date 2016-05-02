trashtalker is a pwnable/"leakable" CTF challenge built for the CrikeyCon 2016 CTF

It also featured in the [BSides Canberra 2016 CTF](https://github.com/IonizeCbr/BSidesCTF) with a different flag, but otherwise identical code.

# Briefing

The file I'm giving you contains a placeholder flag that looks like `not_the_flag{something}`.

The server is running the same binary, but with a real flag that looks like `flag{something}`.

trashtalker is protected by a proof-of-work challenge. Challenges look like `n:s`. You should respond with a string that, when appended to `s`, results in a string that has a SHA256 hash that starts with `n` "0" characters (when the hash is represented as a hex digest). Hashes are calculated without trailing newlines, and you should only send a winning suffix (not the whole string)

Obtain the server's flag, get the points.

Bet you can't.

# Files

* `trashtalker` - distributable binary with the fake flag. This file was given to contestants.
* `trashtalker_priv` - undistributable binary with the real flag, for running on the challenge server (e.g. via xinetd). This file was kept secret.

# Writeups

* [lxb](https://github.com/0x6c7862/ctf-writeups/blob/master/trashtalker.md)

Have you published a writeup? Message me somehow (e.g. I'm [@justinsteven](https://twitter.com/justinsteven) on Twitter) and I'll add it here!

# Warning - this binary is unintentionally vulnerable to a remote memory corruption bug

[@mongobug](https://twitter.com/mongobug) pointed out that I failed to filter for some memory-corrupting format string sequences (using `hn` and `hhn`). The binary is vulnerable to remote memory corruption, and possibly RCE. Do not publicly expose it on a machine you care about.

# License

trashtalker is licensed under Apache License 2.0 (see [LICENSE](LICENSE))

Pre-built binaries include software developed by the OpenSSL Project for use in the OpenSSL Toolkit. (http://www.openssl.org/) (see [openssl-license.txt](openssl-license.txt))
