// -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*-

// Copyright (c) 2001-2006 International Computer Science Institute
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software")
// to deal in the Software without restriction, subject to the conditions
// listed in the XORP LICENSE file. These conditions include: you must
// preserve this copyright notice, and you cannot mention the copyright
// holders in advertising related to the Software without their permission.
// The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
// notice is a summary of the XORP LICENSE file; the license in that file is
// legally binding.

#ident "$XORP: xorp/libxipc/xrl_pf.cc,v 1.3 2005/03/25 02:53:33 pavlin Exp $"

#include "xrl_pf.hh"

// ----------------------------------------------------------------------------
// XrlPFListener

XrlPFListener::XrlPFListener(EventLoop& e, XrlDispatcher* d)
    : _eventloop(e), _dispatcher(d)
{
}

XrlPFListener::~XrlPFListener()
{
}

bool
XrlPFListener::set_dispatcher(const XrlDispatcher* d)
{
    if (_dispatcher == 0) {
	_dispatcher = d;
	return true;
    }
    return false;
}

// ----------------------------------------------------------------------------
// XrlPFSender

XrlPFSender::XrlPFSender(EventLoop& e, const char* address)
    : _eventloop(e), _address(address)
{
}

XrlPFSender::~XrlPFSender()
{
}