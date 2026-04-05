# ft_irc — Internet Relay Chat Server

*This project has been created as part of the 42 curriculum by hchouai, zchtaibi.*

---

## Description

**ft_irc** is a fully functional IRC server implemented in C++98, built as part of the 42 Network curriculum. The goal is to understand and implement the IRC protocol from scratch — handling real TCP/IP connections, non-blocking I/O, and the full lifecycle of a multi-client chat server.

The server supports simultaneous connections from multiple IRC clients without forking, using a single `poll()` call to manage all I/O operations. It was developed and tested against **HexChat** as the reference IRC client.

### Features

- **Authentication** — password-protected server connection
- **Nick & user registration** — `NICK` / `USER` commands
- **Channels** — join, leave, send and receive messages
- **Private messaging** — direct messages between users
- **Operator commands** — channel operators can:
  - `KICK` — eject a user from a channel
  - `INVITE` — invite a user to a channel
  - `TOPIC` — view or change the channel topic
  - `MODE` — manage channel modes:
    - `i` — invite-only
    - `t` — restrict TOPIC to operators
    - `k` — channel key (password)
    - `o` — grant/revoke operator privileges
    - `l` — user limit
- **File transfer (DCC)** — peer-to-peer file sending via DCC SEND / DCC RECV

---

## Technical Choices

- **Language:** C++98 (strict standard compliance, compiled with `-Wall -Wextra -Werror -std=c++98`)
- **I/O multiplexing:** single `poll()` call managing all client sockets — no forking, no blocking
- **Transport:** TCP/IP (IPv4)
- **No external libraries** — pure POSIX sockets and C++98 STL

---

## Instructions

### Requirements

- Linux
- A C++98-compatible compiler (`c++` / `g++` / `clang++`)
- HexChat (or any RFC-compliant IRC client) to connect and test

### Compilation

```bash
make
```

This produces the `ircserv` binary.

### Running the server

```bash
./ircserv <port> <password>
```

| Argument | Description |
|---|---|
| `port` | Port the server listens on (e.g. `6667`) |
| `password` | Connection password clients must provide |

Example:

```bash
./ircserv 6667 mypassword
```

### Connecting with HexChat

1. Open HexChat → **Add** a new network
2. Set server to `127.0.0.1/6667`
3. Enter the server password in the network settings
4. Connect — you should be able to join channels, chat, and transfer files

### Quick test with nc

```bash
nc -C 127.0.0.1 6667
```

You can send raw IRC commands to verify the server handles partial data correctly (use `Ctrl+D` to send data in chunks).

### Makefile rules

```bash
make        # build
make clean  # remove object files
make fclean # remove objects + binary
make re     # fclean + build
```

---

## Resources

### IRC Protocol & Documentation

- [RFC 1459 — Internet Relay Chat Protocol](https://tools.ietf.org/html/rfc1459)
- [RFC 2812 — IRC Client Protocol](https://tools.ietf.org/html/rfc2812)
- [Modern IRC Client Protocol (ircdocs.horse)](https://modern.ircdocs.horse/)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [DCC protocol reference (ircdocs.horse)](https://dcc.ircdocs.horse/)
- [HexChat documentation](https://hexchat.readthedocs.io/)

## Live Demo
👉 [**Open interactive demo →**](https://hchouai.github.io/ft_irc/demo)

### AI Usage

AI tools were used during this project for the following tasks:

- Clarifying RFC 1459 message format and edge cases (e.g. partial packet handling, CRLF termination)
- Suggesting approaches for the `poll()` event loop structure
- Helping draft and review this README

All AI-generated content was reviewed, tested, and validated by both team members before integration. No code was copied without full understanding of its logic.