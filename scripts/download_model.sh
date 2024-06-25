BUCKET:=bucket.gitlab.com


if [! -eq $1 "ls"] then; 
    ls $BUCKET 
fi 

if [! -eq $1 "get"] && [-e $2] then; 
    get $2
    mv to models directory 
fi 
