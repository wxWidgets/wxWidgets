<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" indent="yes" encoding="utf-8"/>

<xsl:param name="locale">en</xsl:param>

<xsl:variable name="uline">_</xsl:variable>
<xsl:variable name="hyphen">-</xsl:variable>

<xsl:template match="/">
<xsl:text>create table if not exists uni_languagematch (desired char, supported char, distance int, oneway char, primary key (desired, supported));
delete from uni_languagematch;
begin;
</xsl:text>

<!--
<paradigmLocales locales="en en_GB es es_419 pt_BR pt_PT"/>
<matchVariable id="$enUS" value="AS+CA+GU+MH+MP+PH+PR+UM+US+VI"/>
<matchVariable id="$cnsar" value="HK+MO"/>
<matchVariable id="$americas" value="019"/>
<matchVariable id="$maghreb" value="MA+DZ+TN+LY+MR+EH"/>
-->

<xsl:for-each select="//languageMatches/languageMatch[not(@alt)]">

<xsl:text>insert into uni_languagematch values ('</xsl:text><xsl:call-template name="string-replace-all">
  <xsl:with-param name="text" select="./@desired" />
  <xsl:with-param name="replace" select="$uline" />
  <xsl:with-param name="by" select="$hyphen" />
</xsl:call-template><xsl:text>', '</xsl:text><xsl:call-template name="string-replace-all">
  <xsl:with-param name="text" select="./@supported" />
  <xsl:with-param name="replace" select="$uline" />
  <xsl:with-param name="by" select="$hyphen" />
</xsl:call-template><xsl:text>', </xsl:text>
<xsl:value-of select="./@distance"/><xsl:text>, '</xsl:text>
<xsl:choose>
  <xsl:when test="./@oneway != ''">
    <xsl:value-of select="./@oneway"/>
  </xsl:when>
  <xsl:otherwise>
    <xsl:text>false</xsl:text>
  </xsl:otherwise>
</xsl:choose><xsl:text>');
</xsl:text>

<!--
<xsl:call-template name="string-replace-all">
  <xsl:with-param name="text" select="./@desired" />
  <xsl:with-param name="replace" select="'$cnsar'" />
  <xsl:with-param name="by" select="'HK+MO'" />
</xsl:call-template>
-->

</xsl:for-each>
<xsl:text>commit;
</xsl:text>

</xsl:template>

<xsl:template name="string-replace-all">
  <xsl:param name="text" />
  <xsl:param name="replace" />
  <xsl:param name="by" />
  <xsl:choose>
    <xsl:when test="contains($text, $replace)">
      <xsl:value-of select="substring-before($text,$replace)" />
      <xsl:value-of select="$by" />
      <xsl:call-template name="string-replace-all">
        <xsl:with-param name="text" select="substring-after($text,$replace)" />
        <xsl:with-param name="replace" select="$replace" />
        <xsl:with-param name="by" select="$by" />
      </xsl:call-template>
    </xsl:when>
    <xsl:otherwise>
      <xsl:value-of select="$text" />
    </xsl:otherwise>
  </xsl:choose>
</xsl:template>

</xsl:stylesheet>
