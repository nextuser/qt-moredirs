#ifndef COPYOPTIONS_H
#define COPYOPTIONS_H


struct CopyOptions
{
    enum DupOption{
        Replace,
        Rename,
        Skip
    }   ;
    DupOption dupOption = Skip;
    bool applyAll;

public:
    CopyOptions();
};

#endif // COPYOPTIONS_H
