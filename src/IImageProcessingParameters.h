#ifndef POTOPOTO_IIMAGEPROCESSINGPARAMETERS_H
#define POTOPOTO_IIMAGEPROCESSINGPARAMETERS_H

class IImageProcessingParameters {
public:
    virtual ~IImageProcessingParameters() = default;

    virtual void Reset() = 0;
    virtual bool operator==(const IImageProcessingParameters& other) const = 0;
    virtual bool operator!=(const IImageProcessingParameters& other) const = 0;
};

#endif //POTOPOTO_IIMAGEPROCESSINGPARAMETERS_H