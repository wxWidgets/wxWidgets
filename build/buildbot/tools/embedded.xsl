<?xml version="1.0" encoding="utf-8"?>

<!--
    Name:       embedded.xsl
    Purpose:    Embedded XSLT
    Author:     Mike Wetherell
    RCS-ID:     $Id$
    Copyright:  (c) 2007 Mike Wetherell
    Licence:    wxWidgets licence
-->

<transform xmlns="http://www.w3.org/1999/XSL/Transform"
           xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
           xmlns:XSL="XSL"
           version="1.0">

<namespace-alias stylesheet-prefix="XSL" result-prefix="xsl"/>

<output indent="yes"/>

<variable
    name="top-level-base"
    select="/*/xsl:import |
            /*/xsl:include |
            /*/xsl:attribute-set |
            /*/xsl:character-map |
            /*/xsl:decimal-format |
            /*/xsl:function |
            /*/xsl:import-schema |
            /*/xsl:key |
            /*/xsl:namespace-alias |
            /*/xsl:output |
            /*/xsl:param |
            /*/xsl:preserve-space |
            /*/xsl:strip-space |
            /*/xsl:variable"/>

<variable
    name="top-level"
    select="$top-level-base |
            /*/xsl:template"/>

<variable
    name="top-level-copy"
    select="$top-level-base |
            /*/xsl:template
                [not(following-sibling::xsl:template/@name = @name)]"/>

<template match="/">
    <XSL:transform>
        <copy-of select="/*/namespace::*"/>

        <for-each select="/*/@xsl:*">
            <attribute name="{local-name()}">
                <value-of select="."/>
            </attribute>
        </for-each>

        <apply-templates mode="copy-xsl" select="$top-level-copy"/>
        <apply-templates select="*"/>
    </XSL:transform>
</template>

<template match="*">
    <variable name="pattern">
        <call-template name="coord-pattern"/>
    </variable>

    <XSL:template match="*[generate-id() = generate-id(document('', /){$pattern})]">
        <copy-of select="namespace::*"/>
        <choose>
            <when test="/*/xsl:template[@name = name(current())]">
                <call-template name="expand"/>
            </when>
            <when test="count($top-level | .) != count($top-level)">
                <call-template name="create-context"/>
            </when>
        </choose>
    </XSL:template>

    <if test="not(self::xsl:*)">
        <apply-templates select="node()"/>
    </if>
</template>

<template match="text()"/>

<template match="@*">
    <copy/>
</template>

<template mode="copy-xsl" match="*[/*/xsl:template/@name = name()]">
    <choose>
        <when test="ancestor::xsl:template[@name = name(current())]">
            <XSL:choose>
                <XSL:when test="name() = '{name()}'">
                    <XSL:copy>
                        <call-template name="copy-xsl-children"/>
                    </XSL:copy>
                </XSL:when>
                <XSL:otherwise>
                    <copy>
                        <call-template name="copy-xsl-children"/>
                    </copy>
                </XSL:otherwise>
            </XSL:choose>
        </when>
        <otherwise>
            <call-template name="expand"/>
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

<template name="expand">
    <variable name="params" select="/*/xsl:template[@name = name(current())]/xsl:param"/>

    <XSL:call-template name="{name()}">
        <if test="$params">
            <XSL:with-param name="{$params[1]/@name}">
                <choose>
                    <when test="ancestor-or-self::xsl:*">
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
            <XSL:with-param name="{name()}">
                <call-template name="avt">
                    <with-param name="string" select="."/>
                </call-template>
            </XSL:with-param>
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
            <choose>
                <when test="$elements[1]/self::xsl:*">
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
    <param name="string"/>

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
