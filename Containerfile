FROM ghcr.io/void-linux/void-glibc-full:latest
RUN xbps-install -Syu fftw-devel libiir1-devel armadillo-devel git gcc make
RUN git clone https://github.com/NerKnutson/libNCPA.git
RUN cd libNCPA && make clean && make 
RUN xbps-install -Syu bash bash-completion 
CMD cd libNCPA/bin && bash
