if &cp | set nocp | endif
let s:cpo_save=&cpo
set cpo&vim
map! <xHome> <Home>
map! <xEnd> <End>
map! <S-xF4> <S-F4>
map! <S-xF3> <S-F3>
map! <S-xF2> <S-F2>
map! <S-xF1> <S-F1>
map! <xF4> <F4>
map! <xF3> <F3>
map! <xF2> <F2>
map! <xF1> <F1>
nmap ,ma :call Man()
nmap ,csg :call Csg()
nmap ,csd :call Csd()
nmap ,csc :call Csc()
nmap ,css :call Css()
nmap ,tj :call Tj()
nmap ,st :call Sts()
map ,0 :b!10
map ,9 :b!9
map ,8 :b!8
map ,7 :b!7
map ,6 :b!6
map ,5 :b!5
map ,4 :b!4
map ,3 :b!3
map ,2 :b!2
map ,1 :b!1
map <F4> 
map <F3> zo
map <F2> v]}zf
map <xHome> <Home>
map <xEnd> <End>
map <S-xF4> <S-F4>
map <S-xF3> <S-F3>
map <S-xF2> <S-F2>
map <S-xF1> <S-F1>
map <xF4> <F4>
map <xF3> <F3>
map <xF2> <F2>
map <xF1> <F1>
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set background=dark
set backspace=2
set cscopeprg=/usr/bin/cscope
set cscopetag
set cscopeverbose
set fileencodings=utf-8,latin1
set formatoptions=tcql
set helplang=en
set history=50
set hlsearch
set path=/usr/include
set ruler
set shiftwidth=4
set smartindent
set softtabstop=4
set tabstop=4
set tags=~/cc/rides/dev/tags
set viminfo='20,\"50
