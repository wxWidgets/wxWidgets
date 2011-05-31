<?xml version="1.0"?>

<!--
    Name:       email.xsl
    Purpose:    Create email address lookup.
    Author:     Mike Wetherell
    RCS-ID:     $Id$
    Copyright:  (c) 2007 Mike Wetherell
    Licence:    wxWindows licence

    Usage: xsltproc -html tools/email.xsl http://svn.wxwidgets.org/users.cgi \
            > config/include/wx-devs.xml
-->

<xsl:transform xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">

<xsl:param name="at"> -at- </xsl:param>

<xsl:output indent="yes"/>

<xsl:template match="/">
    <emaillookup>
        <xsl:apply-templates select="//table/tr">
            <xsl:sort select="normalize-space(td[1])"/>
        </xsl:apply-templates>
    </emaillookup>
</xsl:template>

<xsl:template match="tr">
    <xsl:variable name="id" select="normalize-space(td[1])"/>
    <xsl:variable name="email" select="normalize-space(td[3])"/>

    <xsl:if test="$id and not(contains($id, ' ')) and contains($email, $at)">
        <email id="{$id}">
            <xsl:value-of select="$email"/>
        </email>
    </xsl:if>
</xsl:template>

</xsl:transform>
