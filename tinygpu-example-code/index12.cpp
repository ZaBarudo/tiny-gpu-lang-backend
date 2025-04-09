int branch(int a, int b){
    if(a != b){
        return a;
    }else if(a>b){
        return b;
    }else if (b<a){
        return a;
    }else{
        return b;   
    }
}
