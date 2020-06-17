%!TeX spellcheck = none
\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{HEIAarticle}[2018/05/03 (HEIA-FR) School of Engineering Fribourg - LaTeX Template (unofficial)]

% This class is based on the article class
\LoadClass{report}
\setcounter{secnumdepth}{4}


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                               REQUIRED PACKAGES                                %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\RequirePackage{graphicx}
\RequirePackage[a4paper,%
                margin=2cm,%
                bottom=2cm,%
                top=2cm,%
                includefoot]{geometry}

%% Encoding
\RequirePackage[utf8]{inputenc}

%% Language
\RequirePackage[french]{babel}

%% Font
\RequirePackage[sc]{mathpazo}

%% LaTeX' own graphics handling
\RequirePackage{graphicx}

%% Header and Footer styling
\RequirePackage{fancyhdr}

%% Links
\RequirePackage{hyperref}

%% Code
\RequirePackage[cache=false]{minted}

%% Titling
\RequirePackage{titling}

%% Lists
\RequirePackage{enumitem}

\RequirePackage[skins,minted]{tcolorbox}

\RequirePackage{subfiles}

\RequirePackage{tabularx}



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%                                   COMMANDS                                     %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

\AtBeginDocument{%
  \addtocontents{toc}{\protect\thispagestyle{empty}}%
  \addtocontents{lof}{\protect\thispagestyle{empty}}%
  \addtocontents{abstract}{\protect\thispagestyle{empty}}%
}

\usepackage{etoolbox}
\patchcmd{\chapter}{\thispagestyle{plain}}{\thispagestyle{fancy}}{}{}

\def\@logo{\includegraphics[height=2cm]{images/logos/logo_hta.pdf}}
\def\@subtitle{\empty}
\newcommand*{\subtitle}[1]{\gdef\@subtitle{#1}}
\newcommand*{\logo}[2]{\gdef\@logo{\includegraphics[#2]{#1}}}
\newcommand*{\course}[1]{\gdef\@course{#1}}
\newcommand*{\contact}[1]{\gdef\@contact{#1}}

\newcommand*{\maketitlepage}{
    \begin{titlepage}
        \begin{center}
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            \@logo
            \vspace{1em}
            \newline
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            %\vspace{3em}
            {\Large\textbf{\@course} \par}
            {\Large\textbf{\@date} \par}
            \vspace{2em}
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            %{\Large{Haute École de Technique et d'Architecture Fribourg} \par}
            {\Large{\textbf{Filière Informatique}} \par}
            \vspace{2em}
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            \hrule
            \vspace{3em}
            {\Huge{\textbf{Accélérateur de tour télécom \footnote{\href{https://gitlab.forge.hefr.ch/nicolas.maier/tb_nm}{https://gitlab.forge.hefr.ch/nicolas.maier/tb\_nm}}}} \par}
            \vspace{2em}
            \ifx\@subtitle\empty
            \else
                {\huge\@subtitle \par}

            \fi
            \vspace{2em}
            \hrule
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
            %\vspace{5em}
            \vspace{3em}
            {\Large{\textbf{Nicolas Maier \footnote{\href{mailto:nicolas.maier@edu.hefr.ch}{nicolas.maier@edu.hefr.ch}}}}}
            \vspace{2em}
            
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
            \begin{minipage}[b]{\textwidth}
                \hrule width0.7\textwidth\relax
            \end{minipage}
            
            \vspace{1em}
            
            \begin{center}
                
            
            \begin{tabularx}{0.5\textwidth}{X X}
                \large{Superviseurs} : & \large{\textbf{Jacques Supcik \footnote{\href{mailto:jacques.supcik@hefr.ch}{jacques.supcik@hefr.ch}}}} \\
                & \large{\textbf{Michael Mäder \footnote{\href{mailto:michael.maeder@hefr.ch}{michael.maeder@hefr.ch}}}} \\
                & \\
                \large{Expert} : & \large{\textbf{Frédéric Mauron \footnote{\href{mailto:frederic.mauron@ftth-fr.ch}{frederic.mauron@ftth-fr.ch}}}} \\
                \end{tabularx}
            \end{center}
        
        
            %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
            
        \end{center}

        \vfill{}

        \begin{flushright}
            \includegraphics[height=3.6cm]{images/logos/logo_hes-so.pdf}
        \end{flushright}
    \end{titlepage}
}

\renewcommand*{\maketitle}{
    \begin{center}
            \thispagestyle{plain}
            \@logo \\
            \vspace{2em}
            
            {\Large{\textbf{\@title}} \par}
            \vspace{2em}
            \ifx\@subtitle\empty
            \else
                \vspace{1em}
                {\large\@subtitle \par}

            \fi
            {\large\textsc\@course \par}
            {\large\@date \par}

            \vspace{2em}
            {\@Nicolas Maier \par}
            \@contact
    \end{center}
}

\newcommand*{\makesmallheader}{
    \pagestyle{fancy}
    \fancyhf{}
    \renewcommand{\headrulewidth}{0.5pt}
    \lhead{\@title}
    \cfoot{\thepage}
}

\newcommand*{\makefullheader}{
    \pagestyle{fancy}
    \fancyhf{}
    \setlength{\headheight}{52pt}
    \renewcommand{\headrulewidth}{0.5pt}
    \lhead{\includegraphics[height=1.4cm]{images/logos/logo_hta_short}}
    \rhead{\textbf{\@title} \linebreak \@subtitle \vspace{0.2em}}
    \renewcommand{\footrulewidth}{0.5pt}
    \rfoot{Nicolas Maier}
    \cfoot{\thepage}
}

\newcommand{\maketableofcontent}{
    \cleardoublepage
    \tableofcontents
    \thispagestyle{empty}
    \cleardoublepage
    \setcounter{page}{1}
}

\BeforeBeginEnvironment{myminted}{\vspace{0.8em}}
\AfterEndEnvironment{myminted}{\vspace{0.8em}}

\setlength{\parskip}{0.5em}
\setlength{\parindent}{0em}

\definecolor{mintedbackground}{rgb}{1,1,1}
\definecolor{mintedframe}{rgb}{0.52,0.54,0.66}

\setminted{
    style=manni,
    fontfamily=tt,
    linenos=true,
    numberblanklines=true,
    numbersep=12pt,
    numbersep=5pt,
    gobble=0,
    frame=leftline,
    framesep=2mm,
    funcnamehighlighting=true,
    tabsize=4,
    obeytabs=false,
    mathescape=false
    samepage=false,
    showspaces=false,
    showtabs =false,
    texcl=false,
    baselinestretch=1.2,
    fontsize=\footnotesize,
    breaklines=true,
}

%\renewtcblisting{minted}[3][]{enhanced, listing engine=minted,
%listing only,#1, title=#3, minted language=#2,
%coltitle=mintedbackground!30!black,
%fonttitle=\ttfamily\footnotesize,
%sharp corners, top=0mm, bottom=0mm,
%title code={\path[draw=mintedframe,dashed, fill=mintedbackground](title.south west)--(title.south east);},
%frame code={\path[draw=mintedframe, fill=mintedbackground](frame.south west) rectangle (frame.north east);}
%}



\endinput
