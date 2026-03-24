FROM tbot_base:latest
LABEL authors="nexie"

RUN apt update

RUN apt install -y \
    openssh-server \
    gdbserver \
    gdb \
    rsync

RUN mkdir /var/run/sshd
RUN echo 'root:password' | chpasswd # Set a password for the root user (for demonstration)
RUN sed -i 's/^#PermitRootLogin.*/PermitRootLogin yes/' /etc/ssh/sshd_config
EXPOSE 22
CMD ["/usr/sbin/sshd", "-D"]