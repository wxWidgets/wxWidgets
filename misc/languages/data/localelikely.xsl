<?xml version="1.0"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
<xsl:output method="text" indent="yes" encoding="utf-8"/>

<xsl:param name="locale">en</xsl:param>

<xsl:variable name="uline">_</xsl:variable>
<xsl:variable name="hyphen">-</xsl:variable>

<xsl:template match="/">
<xsl:text>create table if not exists uni_likelysubtags (tagfrom char, tagto char, primary key (tagfrom));
delete from uni_likelysubtags;
begin;
</xsl:text>

<xsl:for-each select="//likelySubtags/likelySubtag[not(@alt)]">
<!--
<xsl:choose>
<xsl:when test=". != ''">
-->
<xsl:text>insert into uni_likelysubtags values ('</xsl:text><xsl:call-template name="string-replace-all">
  <xsl:with-param name="text" select="./@from" />
  <xsl:with-param name="replace" select="$uline" />
  <xsl:with-param name="by" select="$hyphen" />
</xsl:call-template><xsl:text>', '</xsl:text><xsl:call-template name="string-replace-all">
  <xsl:with-param name="text" select="./@to" />
  <xsl:with-param name="replace" select="$uline" />
  <xsl:with-param name="by" select="$hyphen" />
</xsl:call-template><xsl:text>');
</xsl:text>
<!--
</xsl:when>
</xsl:choose>
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
