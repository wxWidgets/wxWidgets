// SPDX-License-Identifier: MIT
/**

Doxygen Awesome
https://github.com/jothepro/doxygen-awesome-css

Copyright (c) 2022 - 2025 jothepro

*/

class DoxygenAwesomeFragmentCopyButton extends HTMLElement {
    constructor() {
        super();
        this.onclick=this.copyContent
    }
    static title = "Copy to clipboard"
    static copyIcon = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24"><path d="M0 0h24v24H0V0z" fill="none"/><path d="M23.04,10.322c0,-2.582 -2.096,-4.678 -4.678,-4.678l-6.918,-0c-2.582,-0 -4.678,2.096 -4.678,4.678c0,-0 0,8.04 0,8.04c0,2.582 2.096,4.678 4.678,4.678c0,-0 6.918,-0 6.918,-0c2.582,-0 4.678,-2.096 4.678,-4.678c0,-0 0,-8.04 0,-8.04Zm-2.438,-0l-0,8.04c-0,1.236 -1.004,2.24 -2.24,2.24l-6.918,-0c-1.236,-0 -2.239,-1.004 -2.239,-2.24l-0,-8.04c-0,-1.236 1.003,-2.24 2.239,-2.24c0,0 6.918,0 6.918,0c1.236,0 2.24,1.004 2.24,2.24Z"/><path d="M5.327,16.748c-0,0.358 -0.291,0.648 -0.649,0.648c0,0 0,0 0,0c-2.582,0 -4.678,-2.096 -4.678,-4.678c0,0 0,-8.04 0,-8.04c0,-2.582 2.096,-4.678 4.678,-4.678l6.918,0c2.168,0 3.994,1.478 4.523,3.481c0.038,0.149 0.005,0.306 -0.09,0.428c-0.094,0.121 -0.239,0.191 -0.392,0.191c-0.451,0.005 -1.057,0.005 -1.457,0.005c-0.238,0 -0.455,-0.14 -0.553,-0.357c-0.348,-0.773 -1.128,-1.31 -2.031,-1.31c-0,0 -6.918,0 -6.918,0c-1.236,0 -2.24,1.004 -2.24,2.24l0,8.04c0,1.236 1.004,2.24 2.24,2.24l0,-0c0.358,-0 0.649,0.29 0.649,0.648c-0,0.353 -0,0.789 -0,1.142Z" style="fill-opacity:0.6;"/></svg>`
    static successIcon = `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" width="24" height="24"><path d="M8.084,16.111c-0.09,0.09 -0.212,0.141 -0.34,0.141c-0.127,-0 -0.249,-0.051 -0.339,-0.141c-0.746,-0.746 -2.538,-2.538 -3.525,-3.525c-0.375,-0.375 -0.983,-0.375 -1.357,0c-0.178,0.178 -0.369,0.369 -0.547,0.547c-0.375,0.375 -0.375,0.982 -0,1.357c1.135,1.135 3.422,3.422 4.75,4.751c0.27,0.27 0.637,0.421 1.018,0.421c0.382,0 0.749,-0.151 1.019,-0.421c2.731,-2.732 10.166,-10.167 12.454,-12.455c0.375,-0.375 0.375,-0.982 -0,-1.357c-0.178,-0.178 -0.369,-0.369 -0.547,-0.547c-0.375,-0.375 -0.982,-0.375 -1.357,0c-2.273,2.273 -9.567,9.567 -11.229,11.229Z"/></svg>`
    static successDuration = 980
    static init() {
        $(function() {
            $(document).ready(function() {
                if(navigator.clipboard) {
                    const fragments = document.getElementsByClassName("fragment")
                    for(const fragment of fragments) {
                        const fragmentWrapper = document.createElement("div")
                        fragmentWrapper.className = "doxygen-awesome-fragment-wrapper"
                        const fragmentCopyButton = document.createElement("doxygen-awesome-fragment-copy-button")
                        fragmentCopyButton.innerHTML = DoxygenAwesomeFragmentCopyButton.copyIcon
                        fragmentCopyButton.title = DoxygenAwesomeFragmentCopyButton.title
                
                        fragment.parentNode.replaceChild(fragmentWrapper, fragment)
                        fragmentWrapper.appendChild(fragment)
                        fragmentWrapper.appendChild(fragmentCopyButton)
            
                    }
                }
            })
        })
    }


    copyContent() {
        const content = this.previousSibling.cloneNode(true)
        // filter out line number from file listings
        content.querySelectorAll(".lineno, .ttc").forEach((node) => {
            node.remove()
        })
        let textContent = content.textContent
        // remove trailing newlines that appear in file listings
        let numberOfTrailingNewlines = 0
        while(textContent.charAt(textContent.length - (numberOfTrailingNewlines + 1)) == '\n') {
            numberOfTrailingNewlines++;
        }
        textContent = textContent.substring(0, textContent.length - numberOfTrailingNewlines)
        navigator.clipboard.writeText(textContent);
        this.classList.add("success")
        this.innerHTML = DoxygenAwesomeFragmentCopyButton.successIcon
        window.setTimeout(() => {
            this.classList.remove("success")
            this.innerHTML = DoxygenAwesomeFragmentCopyButton.copyIcon
        }, DoxygenAwesomeFragmentCopyButton.successDuration);
    }
}

customElements.define("doxygen-awesome-fragment-copy-button", DoxygenAwesomeFragmentCopyButton)
