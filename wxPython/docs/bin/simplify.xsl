<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

<xsl:strip-space elements="*" /> 
<xsl:output method="xml" omit-xml-declaration="yes" indent="yes" />

            
<!-- Stuff to ignore (ignored because the rules don't do anything) -->
<xsl:template match="/top/attributelist" />
<xsl:template match="include/attributelist" />
<xsl:template match="namespace" />
<xsl:template match="typemap" />
<xsl:template match="typemapcopy" />
<xsl:template match="insert" />
<xsl:template match="fragment" />
<xsl:template match="constant" />
<xsl:template match="import" /> 


<!-- Wrap the whole thing in a top level element -->
<xsl:template match="/">
    <xsl:element name="module">
        <xsl:attribute name="name"><xsl:value-of select="top/attributelist/attribute[@name='module']/@value"/></xsl:attribute>
        <xsl:apply-templates />
    </xsl:element>
</xsl:template>



<xsl:template match="/top/include/import/module">
    <xsl:element name="import">
        <xsl:attribute name="name"><xsl:value-of select="./attributelist/attribute[@name='name']/@value"/></xsl:attribute>
    </xsl:element>
</xsl:template>


<!-- A callable template that outputs the various docstrings for the current node -->
<xsl:template name="DoDocstrings">
    <xsl:if test="./attributelist/attribute[@name='python_autodoc' and @value!='']">
        <xsl:element name="autodoc"><xsl:value-of select="./attributelist/attribute[@name='python_autodoc']/@value"/></xsl:element>
    </xsl:if>
    <xsl:if test="./attributelist/attribute[@name='feature_docstring' and @value!='']">
        <xsl:element name="docstring"><xsl:value-of select="./attributelist/attribute[@name='feature_docstring']/@value"/></xsl:element>
    </xsl:if>
    <xsl:if test="./attributelist/attribute[@name='feature_refdoc' and @value!='']">
        <xsl:element name="refdoc"><xsl:value-of select="./attributelist/attribute[@name='feature_refdoc']/@value"/></xsl:element>
    </xsl:if>
</xsl:template>


<!-- A callable template that handles parameter lists -->
<xsl:template name="DoParamList">
    <xsl:if test="attributelist/parmlist">
        <xsl:element name="paramlist">
            <xsl:for-each select="attributelist/parmlist/parm">
	        <xsl:element name="param">
		    <xsl:attribute name="name"><xsl:value-of select="./attributelist/attribute[@name='name']/@value"/></xsl:attribute>
		    <xsl:attribute name="type"><xsl:value-of select="./attributelist/attribute[@name='type']/@value"/></xsl:attribute>
		    <xsl:attribute name="default"><xsl:value-of select="./attributelist/attribute[@name='value']/@value"/></xsl:attribute>
		</xsl:element>
  	    </xsl:for-each>
	</xsl:element>
    </xsl:if>
</xsl:template>


<!-- Check for overloaded methods -->
<xsl:template name="CheckOverloaded">
    <xsl:choose>
        <xsl:when test="./attributelist/attribute[@name='sym_overloaded']">
	   <xsl:attribute name="overloaded">yes</xsl:attribute>
        </xsl:when>
        <xsl:otherwise>
	   <xsl:attribute name="overloaded">no</xsl:attribute>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>



<!-- A callable template that handles functions, methods, etc. -->
<xsl:template name="DoFunction">
    <xsl:attribute name="name"><xsl:value-of select="./attributelist/attribute[@name='sym_name']/@value"/></xsl:attribute>
    <!-- <xsl:attribute name="returntype"><xsl:value-of select="./attributelist/attribute[@name='type']/@value"/></xsl:attribute> -->
    <xsl:call-template name="CheckOverloaded" />
    <xsl:call-template name="DoDocstrings" />
    <xsl:call-template name="DoParamList" />	        
</xsl:template>




<!-- Create a class element with doc sub elements taken from the attributelist -->
<xsl:template match="class">
    <xsl:element name="class">

        <xsl:attribute name="name">
	    <xsl:value-of select="./attributelist/attribute[@name='sym_name']/@value"/>
        </xsl:attribute>
        <xsl:attribute name="oldname">
	    <xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>
        </xsl:attribute>

        <xsl:attribute name="module">
	    <xsl:value-of select="./attributelist/attribute[@name='module']/@value"/>
        </xsl:attribute>

	<xsl:call-template name="DoDocstrings" />
        <xsl:apply-templates />

    </xsl:element>
</xsl:template>

<xsl:template match="base">
    <xsl:element name="baseclass">
        <xsl:attribute name="name">
	    <xsl:value-of select="@name"/>
        </xsl:attribute>
    </xsl:element>
</xsl:template>


<!-- constructor -->
<xsl:template match="constructor">
    <xsl:element name="constructor">
        <xsl:attribute name="name">
	    <xsl:value-of select="./attributelist/attribute[@name='sym_name']/@value"/>
        </xsl:attribute>
	<xsl:call-template name="CheckOverloaded" />

	<xsl:call-template name="DoDocstrings" />
	<xsl:call-template name="DoParamList" />

    </xsl:element>
</xsl:template>


<!-- destructor -->
<xsl:template match="destructor">
    <xsl:element name="destructor">

        <xsl:attribute name="name">
	    <xsl:value-of select="./attributelist/attribute[@name='sym_name']/@value"/>
        </xsl:attribute>

	<xsl:call-template name="DoDocstrings" />
	<xsl:call-template name="DoParamList" />

    </xsl:element>
</xsl:template>


<!-- cdecls: can be functions, methods, properties, etc. -->
<xsl:template match="cdecl">
    <xsl:choose>
        <!-- method -->
        <xsl:when test="./attributelist/attribute[@name='view' and @value='memberfunctionHandler']">
	    <xsl:element name="method">
	        <xsl:call-template name="DoFunction" />
	    </xsl:element>
        </xsl:when>

        <!-- staticmethod -->
        <xsl:when test="./attributelist/attribute[@name='view' and @value='staticmemberfunctionHandler']">
	    <xsl:element name="staticmethod">
	        <xsl:call-template name="DoFunction" />
	    </xsl:element>
        </xsl:when>
	
	<!-- property -->
	<xsl:when test="./attributelist/attribute[@name='view' and @value='variableHandler']">
	    <xsl:element name="property">
	        <xsl:attribute name="name">
		    <xsl:value-of select="./attributelist/attribute[@name='sym_name']/@value"/>
		</xsl:attribute>
	        <xsl:attribute name="type">
		    <xsl:value-of select="./attributelist/attribute[@name='type']/@value"/>
		</xsl:attribute>
		<xsl:choose>
		    <xsl:when test="./attributelist/attribute[@name='feature_immutable']">
		        <xsl:attribute name="readonly">yes</xsl:attribute>
	            </xsl:when>
	            <xsl:otherwise>
		        <xsl:attribute name="readonly">no</xsl:attribute>
	            </xsl:otherwise>
		</xsl:choose>
        	<xsl:call-template name="DoDocstrings" />
	    </xsl:element>
        </xsl:when>

        <!-- global function -->
        <xsl:when test="./attributelist/attribute[@name='view' and @value='globalfunctionHandler']">
	    <xsl:element name="function">
                <xsl:attribute name="oldname">
	            <xsl:value-of select="./attributelist/attribute[@name='name']/@value"/>
                </xsl:attribute>
	        <xsl:call-template name="DoFunction" />
	    </xsl:element>
        </xsl:when>

    </xsl:choose>
</xsl:template>


<!-- %pythoncode directives -->
<xsl:template match="insert">
    <xsl:if test="./attributelist/attribute[@name='section' and @value='python']">
        <xsl:element name="pythoncode">
	    <xsl:value-of select="./attributelist/attribute[@name='code']/@value"/>
        </xsl:element>
    </xsl:if>
</xsl:template>


</xsl:stylesheet>

