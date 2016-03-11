
include_directories(${CMAKE_SOURCE_DIR}/Source/ThirdParty/zlib)

if (ATOMIC_PROJECT_ARCH STREQUAL "x86")
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/curl/windows/32bit)
else()
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/curl/windows/64bit)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/lib)

add_definitions(-DBUILDING_LIBCURL -D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE -D_WIN32_WINNT=0x0501)
add_definitions(-DUSE_SCHANNEL -DUSE_WINDOWS_SSPI -DHAVE_CONFIG_H)

set (LIBCURL_SOURCE_FILES lib/file.c
    lib/timeval.c
    lib/base64.c
    lib/hostip.c
    lib/progress.c
    lib/formdata.c
    lib/cookie.c
    lib/http.c
    lib/sendf.c
    lib/ftp.c
    lib/url.c
    lib/dict.c
    lib/if2ip.c
    lib/speedcheck.c
    lib/ldap.c
    lib/version.c
    lib/getenv.c
    lib/escape.c
    lib/mprintf.c
    lib/telnet.c
    lib/netrc.c
    lib/getinfo.c
    lib/transfer.c
    lib/strequal.c
    lib/easy.c
    lib/security.c
    lib/curl_fnmatch.c
    lib/fileinfo.c
    lib/ftplistparser.c
    lib/wildcard.c
    lib/krb5.c
    lib/memdebug.c
    lib/http_chunks.c
    lib/strtok.c
    lib/connect.c
    lib/llist.c
    lib/hash.c
    lib/multi.c
    lib/content_encoding.c
    lib/share.c
    lib/http_digest.c
    lib/md4.c
    lib/md5.c
    lib/http_negotiate.c
    lib/inet_pton.c
    lib/strtoofft.c
    lib/strerror.c
    lib/amigaos.c
    lib/hostasyn.c
    lib/hostip4.c
    lib/hostip6.c
    lib/hostsyn.c
    lib/inet_ntop.c
    lib/parsedate.c
    lib/select.c
    lib/tftp.c
    lib/splay.c
    lib/strdup.c
    lib/socks.c
    lib/ssh.c
    lib/rawstr.c
    lib/curl_addrinfo.c
    lib/socks_gssapi.c
    lib/socks_sspi.c
    lib/curl_sspi.c
    lib/slist.c
    lib/nonblock.c
    lib/curl_memrchr.c
    lib/imap.c
    lib/pop3.c
    lib/smtp.c
    lib/pingpong.c
    lib/rtsp.c
    lib/curl_threads.c
    lib/warnless.c
    lib/hmac.c
    lib/curl_rtmp.c
    lib/openldap.c
    lib/curl_gethostname.c
    lib/gopher.c
    lib/idn_win32.c
    lib/http_negotiate_sspi.c
    lib/http_proxy.c
    lib/non-ascii.c
    lib/asyn-ares.c
    lib/asyn-thread.c
    lib/curl_gssapi.c
    lib/curl_ntlm.c
    lib/curl_ntlm_wb.c
    lib/curl_ntlm_core.c
    lib/curl_ntlm_msgs.c
    lib/curl_sasl.c
    lib/curl_multibyte.c
    lib/hostcheck.c
    lib/bundles.c
    lib/conncache.c
    lib/pipeline.c
    lib/dotdot.c
    lib/x509asn1.c
    lib/http2.c
    lib/curl_sasl_sspi.c
    lib/smb.c
    lib/curl_sasl_gssapi.c
    lib/curl_endian.c
    lib/vtls/openssl.c
    lib/vtls/gtls.c
    lib/vtls/vtls.c
    lib/vtls/nss.c
    lib/vtls/polarssl.c
    lib/vtls/polarssl_threadlock.c
    lib/vtls/axtls.c
    lib/vtls/cyassl.c
    lib/vtls/curl_schannel.c
    lib/vtls/curl_darwinssl.c
    lib/vtls/gskit.c )

add_library(libcurl ${LIBCURL_SOURCE_FILES} )

target_link_libraries(libcurl zlib)
