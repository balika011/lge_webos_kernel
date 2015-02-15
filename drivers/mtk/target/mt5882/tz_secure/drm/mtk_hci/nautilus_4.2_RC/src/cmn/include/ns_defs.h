/*
 * Nautilus version 4.2
 *
 * Any code and mechanism in this module may not be used
 * in any form without permissions.
 * Copyright 2006,2007,2008,2009,2010,2011,2012 Sony Corporation.
 */


/* This file contains all XML namespaces used by the Nautilus.
 * Note that all the namespaces have to be defined only in this file. */


#ifndef NS_DEFS_H
#define NS_DEFS_H

#define NS_PKT \
    "http://www.octopus-drm.com/profiles/base/1.0/plankton"

#define NS_OCT \
    "http://www.octopus-drm.com/profiles/base/1.0"

#define NS_SF \
    "http://marlin-drm.com/starfish/1.2"

#define NS_NEMOC \
    "http://nemo.intertrust.com/2005/10/core"

#define NS_NEMOSEC \
    "http://nemo.intertrust.com/2005/10/security"

#define NS_XSD \
    "http://www.w3.org/2001/XMLSchema"

#define NS_DS \
    "http://www.w3.org/2000/09/xmldsig#"
 
#define NS_XENC \
    "http://www.w3.org/2001/04/xmlenc#"

#define NS_WSDL \
    "http://schemas.xmlsoap.org/wsdl/"

#define NS_WSA \
    "http://www.w3.org/2005/08/addressing"

#define NS_WSX \
    "http://schemas.xmlsoap.org/ws/2004/09/mex"

#define NS_S11 \
    "http://schemas.xmlsoap.org/soap/envelope/"

#define NS_S12 \
    "http://www.w3.org/2003/05/soap-envelope"

#define NS_SOAP NS_S11    /* default SOAP namespaces */

#define NS_WSU \
    "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd"

#define NS_WSSE \
    "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd"

#define NS_WSP \
    "http://schemas.xmlsoap.org/ws/2004/09/policy"

#define NS_WST \
    "http://schemas.xmlsoap.org/ws/2004/04/trust"

#define NS_MC \
    "urn:marlin:core:1-3:schemas"

#define NS_MNCS \
    "urn:marlin:core:1-1:nemo:services:schemas"

#define NS_EXC \
    "urn:marlin:core:1-2:nemo:services:schemas:exceptions"

#define NS_SAML \
    "urn:oasis:names:tc:SAML:1.0:assertion"

#define NS_LS \
    "urn:marlin:broadband:1-1:nemo:services:schemas:license-service"

#define NS_RS \
    "urn:marlin:broadband:1-1:nemo:services:schemas:registration-service"

#define NS_DCS \
    "urn:marlin:broadband:1-1:nemo:services:schemas:data-certification-service"

#define NS_DUS \
    "urn:marlin:broadband:1-2:nemo:services:schemas:data-update-service"

#define NS_MDS \
    "urn:marlin:broadband:1-1:nemo:services:schemas:metering-service"

#define NS_DCSI \
    "urn:marlin:broadband:1-1:nemo:services:schemas:data-certification-service:data-item"

#define NS_BSA \
    "urn:marlin:broadband:1-2:nemo:services:action-token"

#define NS_BSA_MS3 \
    "urn:marlin:ms3:1-0:services:schemas:streaming:action-token"

#define NS_BSC \
    "urn:marlin:broadband:1-2:nemo:services:configuration"

#define NS_MAS \
    "urn:marlin:mas:1-0:services:schemas:mpd"

#define NS_BROADCASTKEY \
    "http://marlin-drm.com/1.0"

#endif

