# OS-Project

This project is a terminal oriented project which implements client – server based online
store application. The user / admin can login from the client portal and the server is
responsible for handling the requests sent by the client. The connection between the client
and the sever is implemented using socket programming. The server is also a concurrent
server i.e., it can handle multiple clients simultaneously. This was implemented by creating a
new fork for every new client. Finally, the concept of file locking has been implemented so
that multiple users cannot change or access the same file or record in the table as otherwise
it might lead to date inconsistency.
