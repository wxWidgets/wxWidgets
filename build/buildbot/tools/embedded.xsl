<?xml version="1.0" encoding="utf-8"?>

<!--
    Name:       embedded.xsl
    Purpose:    Used by check.sh for offline checking of the configuration.
    Author:     Mike Wetherell
    RCS-ID:     $Id$
    Copyright:  (c) 2007 Mike Wetherell
    Licence:    wxWindows licence
-->

<transform xmlns="http://www.w3.org/1999/XSL/Transform"
           xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
           xmlns:XSL="XSL"
           version="1.0">

<namespace-alias stylesheet-prefix="XSL" result-prefix="xsl"/>

<output indent="yes"/>

<variable
    name="root"
    select="//*[not(ancestor-or-self::*[name() != name(/*)])]"/>

<variable
    name="includes"
    select="$root[position() > 1]"/>

<variable
    xmlns:func="http://exslt.org/functions"
    name="top-level"
    select="$root/xsl:import |
            $root/xsl:include |
            $root/xsl:attribute-set |
            $root/xsl:character-map |
            $root/xsl:decimal-format |
            $root/xsl:function |
            $root/xsl:import-schema |
            $root/xsl:key |
            $root/xsl:namespace-alias |
            $root/xsl:output |
            $root/xsl:param |
            $root/xsl:preserve-space |
            $root/xsl:strip-space |
            $root/xsl:template |
            $root/xsl:variable |
            $root/func:function"/>

<template match="/">
    <XSL:transform>
        <copy-of select="$root/namespace::*"/>

        <for-each select="$root/@xsl:*">
            <attribute name="{local-name()}">
                <value-of select="."/>
            </attribute>
        </for-each>

        <for-each select="$top-level">
            <if test="not(../ancestor::*/*[name() = name(current()) and @name = current()/@name])">
                <apply-templates mode="copy-xsl" select="."/>
            </if>
        </for-each>

        <apply-templates select="*"/>
    </XSL:transform>
</template>

<template match="*">
    <variable name="pattern">
        <call-template name="coord-pattern"/>
    </variable>

    <variable name="is-xsl">
        <call-template name="is-xsl"/>
    </variable>

    <XSL:template match="*[generate-id() = generate-id(document('', /){$pattern})]">
        <copy-of select="namespace::*"/>
        <choose>
            <when test="$root/xsl:template[@name = name(current())]">
                <call-template name="expand">
                    <with-param name="inside-xsl" select="$is-xsl = 'true'"/>
                </call-template>
            </when>
            <when test="count($includes | .) = count($includes)">
                <if test="node()">
                    <XSL:apply-templates select="node()"/>
                </if>
            </when>
            <when test="count($top-level | .) != count($top-level)">
                <call-template name="create-context"/>
            </when>
        </choose>
    </XSL:template>

    <if test="$is-xsl != 'true'">
        <apply-templates select="node()"/>
    </if>
</template>

<template match="text()"/>

<template match="@*">
    <copy/>
</template>

<template mode="copy-xsl" match="*[//xsl:template[not(ancestor::*[name() != name(/*)])]/@name = name()]">
    <choose>
        <when test="ancestor::xsl:template[@name = name(current())]">
            <copy>
                <call-template name="copy-xsl-children"/>
            </copy>
        </when>
        <otherwise>
            <call-template name="expand">
                <with-param name="inside-xsl" select="true()"/>
            </call-template>
        </otherwise>
    </choose>
</template>

<template mode="copy-xsl" match="@*|node()">
    <copy>
        <call-template name="copy-xsl-children"/>
    </copy>
</template>

<template name="copy-xsl-children">
    <copy-of select="namespace::*"/>
    <apply-templates mode="copy-xsl" select="@*|node()"/>
</template>

<template name="coord-pattern">
    <param name="element" select="."/>

    <for-each select="$element/ancestor-or-self::*">
        <text>/*[</text>
        <value-of select="count(preceding-sibling::*) + 1"/>
        <text>]</text>
    </for-each>
</template>

<template name="is-xsl">
    <param name="element" select="."/>
    <choose>
        <when test="$element/self::xsl:*">
            <value-of select="true()"/>
        </when>
        <otherwise>
            <variable name="namespace" select="namespace-uri($element)"/>
            <variable name="extension-namespaces">
                <for-each select="$element/ancestor-or-self::*/@xsl:extension-element-prefixes">
                    <variable name="prefixes"
                              select="concat(' ', normalize-space(.), ' ')"/>
                    <variable name="namespaces"
                              select="../namespace::*[contains($prefixes, concat(' ', name(), ' ')) or
                                                      (name() = '' and contains($prefixes, ' #default '))]"/>
                    <value-of select="$namespaces[. = $namespace]"/>
                </for-each>
            </variable>
            <value-of select="$extension-namespaces != ''"/>
        </otherwise>
    </choose>
</template>

<template name="expand">
    <param name="inside-xsl"/>
    <variable name="params" select="$root/xsl:template[@name = name(current())]/xsl:param"/>

    <XSL:call-template name="{name()}">
        <if test="node() and $params">
            <XSL:with-param name="{$params[1]/@name}">
                <choose>
                    <when test="$inside-xsl">
                        <apply-templates mode="copy-xsl" select="node()"/>
                    </when>
                    <otherwise>
                        <variable name="pattern">
                            <call-template name="coord-pattern"/>
                        </variable>
                        <XSL:apply-templates select="{$pattern}/node()"/>
                    </otherwise>
                </choose>
            </XSL:with-param>
        </if>

        <for-each select="@*">
            <variable name="len" select="string-length(.)"/>
            <choose>
                <when test="substring(., 1, 1) = '{' and
                            substring(., $len, 1) = '}'">
                    <XSL:with-param name="{name()}"
                                    select="{substring(., 2, $len - 2)}"/>
                </when>
                <otherwise>
                    <XSL:with-param name="{name()}">
                        <call-template name="avt"/>
                    </XSL:with-param>
                </otherwise>
            </choose>
        </for-each>
    </XSL:call-template>
</template>

<template name="create-context">
    <param name="elements" select="ancestor-or-self::*
                                   [last() - 1 > position()]
                                   /preceding-sibling::xsl:variable | ."/>

    <variable name="pattern">
        <call-template name="coord-pattern">
            <with-param name="element" select="$elements[1]"/>
        </call-template>
    </variable>

    <variable name="parent-pattern">
        <call-template name="coord-pattern">
            <with-param name="element" select="$elements[1]/.."/>
        </call-template>
    </variable>

    <XSL:for-each select="{$parent-pattern}/*">
        <XSL:if test="count({$pattern} | .) = 1">
            <variable name="is-xsl">
                <call-template name="is-xsl">
                    <with-param name="element" select="$elements[1]"/>
                </call-template>
            </variable>
            <choose>
                <when test="$is-xsl = 'true'">
                    <apply-templates mode="copy-xsl" select="$elements[1]"/>
                    <if test="$elements[2]">
                        <call-template name="create-context">
                            <with-param name="elements" select="$elements[position() > 1]"/>
                        </call-template>
                    </if>
                </when>
                <otherwise>
                    <copy>
                        <apply-templates select="@*"/>
                        <if test="node()">
                            <XSL:apply-templates select="node()"/>
                        </if>
                    </copy>
                </otherwise>
            </choose>
        </XSL:if>
    </XSL:for-each>
</template>

<template name="before">
    <param name="string"/>
    <param name="target"/>

    <variable name="apos">'</variable>
    <variable name="quot">"</variable>

    <variable name="posapos" select="string-length(substring-before(
                                        concat($string, $apos), $apos))"/>
    <variable name="posquot" select="string-length(substring-before(
                                        concat($string, $quot), $quot))"/>
    <variable name="postarg" select="string-length(substring-before(
                                        concat($string, $target), $target))"/>

    <choose>
        <when test="$posapos = $postarg and $posquot = $postarg">
            <value-of select="$string"/>
        </when>
        <when test="$posapos > $postarg and $posquot > $postarg">
            <value-of select="substring($string, 1, $postarg)"/>
        </when>
        <otherwise>
            <variable name="delim">
                <choose>
                    <when test="$posquot > $posapos">'</when>
                    <otherwise>"</otherwise>
                </choose>
            </variable>

            <value-of select="substring-before($string, $delim)"/>
            <value-of select="$delim"/>

            <variable name="mid" select="substring-after($string, $delim)"/>

            <choose>
                <when test="contains($mid, $delim)">
                    <value-of select="substring-before($mid, $delim)"/>
                    <value-of select="$delim"/>

                    <call-template name="before">
                        <with-param name="string"
                                    select="substring-after($mid, $delim)"/>
                        <with-param name="target"
                                    select="$target"/>
                    </call-template>
                </when>
                <otherwise>
                    <value-of select="$mid"/>
                </otherwise>
            </choose>
        </otherwise>
    </choose>
</template>

<template name="avt">
    <param name="string" select="."/>

    <variable name="before1" select="substring-before(concat($string, '{'), '{')"/>
    <variable name="len1" select="string-length($before1)"/>

    <variable name="before2" select="substring-before(concat($string, '}}'), '}}')"/>
    <variable name="len2" select="string-length($before2)"/>

    <choose>
        <when test="$before1 = $string and $before2 = $string">
            <value-of select="$string"/>
        </when>
        <when test="$len2 &lt; $len1">
            <value-of select="$before2"/>
            <text>}</text>
            <call-template name="avt">
                <with-param name="string" select="substring($string, $len2 + 3)"/>
            </call-template>
        </when>
        <when test="substring($string, $len1 + 2, 1) = '{'">
            <value-of select="$before1"/>
            <text>{</text>
            <call-template name="avt">
                <with-param name="string" select="substring($string, $len1 + 3)"/>
            </call-template>
        </when>
        <otherwise>
            <variable name="mid" select="substring($string, $len1 + 2)"/>
            <variable name="expr">
                <call-template name="before">
                    <with-param name="string" select="$mid"/>
                    <with-param name="target">}</with-param>
                </call-template>
            </variable>
            <value-of select="$before1"/>
            <XSL:copy-of select="{$expr}"/>
            <call-template name="avt">
                <with-param name="string" select="substring($mid, string-length($expr) + 2)"/>
            </call-template>
        </otherwise>
    </choose>
</template>

</transform>
