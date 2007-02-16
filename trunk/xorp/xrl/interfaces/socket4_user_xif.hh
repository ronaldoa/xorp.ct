/*
 * Copyright (c) 2001-2007 International Computer Science Institute
 * See LICENSE file for licensing, conditions, and warranties on use.
 *
 * DO NOT EDIT THIS FILE - IT IS PROGRAMMATICALLY GENERATED
 *
 * Generated by 'clnt-gen'.
 *
 * $XORP: xorp/xrl/interfaces/socket4_user_xif.hh,v 1.9 2006/03/27 00:09:19 pavlin Exp $
 */

#ifndef __XRL_INTERFACES_SOCKET4_USER_XIF_HH__
#define __XRL_INTERFACES_SOCKET4_USER_XIF_HH__

#undef XORP_LIBRARY_NAME
#define XORP_LIBRARY_NAME "XifSocket4User"

#include "libxorp/xlog.h"
#include "libxorp/callback.hh"

#include "libxipc/xrl.hh"
#include "libxipc/xrl_error.hh"
#include "libxipc/xrl_sender.hh"


class XrlSocket4UserV0p1Client {
public:
    XrlSocket4UserV0p1Client(XrlSender* s) : _sender(s) {}
    virtual ~XrlSocket4UserV0p1Client() {}

    typedef XorpCallback1<void, const XrlError&>::RefPtr RecvEventCB;
    /**
     *  Send Xrl intended to:
     *
     *  Method invoked by target implementing socket4/0.1 when a packet arrives
     *  from an IPv4 source.
     *
     *  @param dst_xrl_target_name the Xrl target name of the destination.
     *
     *  @param sockid the identifier associated with socket where error
     *  occurred.
     *
     *  @param src_host the originating host.
     *
     *  @param src_port the originating IP port.
     *
     *  @param data the data received.
     */
    bool send_recv_event(
	const char*	dst_xrl_target_name,
	const string&	sockid,
	const IPv4&	src_host,
	const uint32_t&	src_port,
	const vector<uint8_t>&	data,
	const RecvEventCB&	cb
    );

    typedef XorpCallback2<void, const XrlError&, const bool*>::RefPtr ConnectEventCB;
    /**
     *  Send Xrl intended to:
     *
     *  Method invoked by target implementing socket4/0.1 when a connection
     *  request is received from an IPv4 source.
     *
     *  @param dst_xrl_target_name the Xrl target name of the destination.
     */
    bool send_connect_event(
	const char*	dst_xrl_target_name,
	const string&	sockid,
	const IPv4&	src_host,
	const uint32_t&	src_port,
	const string&	new_sockid,
	const ConnectEventCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr ErrorEventCB;
    /**
     *  Send Xrl intended to:
     *
     *  Method invoked by target implementing socket4/0.1 when an error occurs.
     *
     *  @param dst_xrl_target_name the Xrl target name of the destination.
     *
     *  @param sockid the identifier associated with socket where error
     *  occurred.
     *
     *  @param error a textual description of the error.
     *
     *  @param fatal indication of whether socket is shutdown because of error.
     */
    bool send_error_event(
	const char*	dst_xrl_target_name,
	const string&	sockid,
	const string&	error,
	const bool&	fatal,
	const ErrorEventCB&	cb
    );

    typedef XorpCallback1<void, const XrlError&>::RefPtr CloseEventCB;
    /**
     *  Send Xrl intended to:
     *
     *  Method invoked by target implementing socket4/0.1 when a socket is
     *  forcibly closed. This would typically happen when the address the
     *  socket is bound to is removed from an interface. This method is not
     *  called if the socket is closed through socket/0.1/close.
     *
     *  @param dst_xrl_target_name the Xrl target name of the destination.
     */
    bool send_close_event(
	const char*	dst_xrl_target_name,
	const string&	sockid,
	const string&	reason,
	const CloseEventCB&	cb
    );

protected:
    XrlSender* _sender;

private:
    void unmarshall_recv_event(
	const XrlError&	e,
	XrlArgs*	a,
	RecvEventCB		cb
    );

    void unmarshall_connect_event(
	const XrlError&	e,
	XrlArgs*	a,
	ConnectEventCB		cb
    );

    void unmarshall_error_event(
	const XrlError&	e,
	XrlArgs*	a,
	ErrorEventCB		cb
    );

    void unmarshall_close_event(
	const XrlError&	e,
	XrlArgs*	a,
	CloseEventCB		cb
    );

};

#endif /* __XRL_INTERFACES_SOCKET4_USER_XIF_HH__ */
