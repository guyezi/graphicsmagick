#ifndef _LOCAL_C_H
#define _LOCAL_C_H

extern MagickExport const char *GetLocaleMessageFromID(const int);

#define MAX_LOCALE_MSGS 571

#define MGK_BlobErrorUnableToCreateBlob 1
#define MGK_BlobErrorUnableToDeduceImageFormat 2
#define MGK_BlobErrorUnableToObtainOffset 3
#define MGK_BlobErrorUnableToOpenFile 4
#define MGK_BlobErrorUnableToReadFile 5
#define MGK_BlobErrorUnableToReadToOffset 6
#define MGK_BlobErrorUnableToSeekToOffset 7
#define MGK_BlobErrorUnableToWriteBlob 8
#define MGK_BlobErrorUnrecognizedImageFormat 9
#define MGK_BlobFatalErrorDefault 10
#define MGK_BlobWarningDefault 11
#define MGK_CacheErrorInconsistentPersistentCacheDepth 12
#define MGK_CacheErrorPixelCacheDimensionsMisMatch 13
#define MGK_CacheErrorPixelCacheIsNotOpen 14
#define MGK_CacheErrorUnableToAllocateCacheView 15
#define MGK_CacheErrorUnableToCloneCache 16
#define MGK_CacheErrorUnableToExtendCache 17
#define MGK_CacheErrorUnableToGetCacheNexus 18
#define MGK_CacheErrorUnableToGetPixelsFromCache 19
#define MGK_CacheErrorUnableToOpenCache 20
#define MGK_CacheErrorUnableToPeristPixelCache 21
#define MGK_CacheErrorUnableToReadPixelCache 22
#define MGK_CacheErrorUnableToSyncCache 23
#define MGK_CacheFatalErrorDiskAllocationFailed 24
#define MGK_CacheFatalErrorUnableToExtendPixelCache 25
#define MGK_CacheWarningDefault 26
#define MGK_CoderErrorArithmeticOverflow 27
#define MGK_CoderErrorColormapTooLarge 28
#define MGK_CoderErrorColormapTypeNotSupported 29
#define MGK_CoderErrorColorspaceModelIsNotSupported 30
#define MGK_CoderErrorColorTypeNotSupported 31
#define MGK_CoderErrorCompressionNotValid 32
#define MGK_CoderErrorDataEncodingSchemeIsNotSupported 33
#define MGK_CoderErrorDataStorageTypeIsNotSupported 34
#define MGK_CoderErrorDeltaPNGNotSupported 35
#define MGK_CoderErrorDivisionByZero 36
#define MGK_CoderErrorEncryptedWPGImageFileNotSupported 37
#define MGK_CoderErrorFractalCompressionNotSupported 38
#define MGK_CoderErrorImageColumnOrRowSizeIsNotSupported 39
#define MGK_CoderErrorImageDoesNotHaveAMatteChannel 40
#define MGK_CoderErrorImageIsNotTiled 41
#define MGK_CoderErrorImageTypeNotSupported 42
#define MGK_CoderErrorIncompatibleSizeOfDouble 43
#define MGK_CoderErrorIrregularChannelGeometryNotSupported 44
#define MGK_CoderErrorJNGCompressionNotSupported 45
#define MGK_CoderErrorJPEGCompressionNotSupported 46
#define MGK_CoderErrorJPEGEmbeddingFailed 47
#define MGK_CoderErrorLocationTypeIsNotSupported 48
#define MGK_CoderErrorMapStorageTypeIsNotSupported 49
#define MGK_CoderErrorMSBByteOrderNotSupported 50
#define MGK_CoderErrorMultidimensionalMatricesAreNotSupported 51
#define MGK_CoderErrorMultipleRecordListNotSupported 52
#define MGK_CoderErrorNo8BIMDataIsAvailable 53
#define MGK_CoderErrorNoAPP1DataIsAvailable 54
#define MGK_CoderErrorNoBitmapOnClipboard 55
#define MGK_CoderErrorNoColorProfileAvailable 56
#define MGK_CoderErrorNoDataReturned 57
#define MGK_CoderErrorNoImageVectorGraphics 58
#define MGK_CoderErrorNoIPTCInfoWasFound 59
#define MGK_CoderErrorNoIPTCProfileAvailable 60
#define MGK_CoderErrorNumberOfImagesIsNotSupported 61
#define MGK_CoderErrorOnlyContinuousTonePictureSupported 62
#define MGK_CoderErrorOnlyLevelZerofilesSupported 63
#define MGK_CoderErrorPNGCompressionNotSupported 64
#define MGK_CoderErrorPNGLibraryTooOld 65
#define MGK_CoderErrorRLECompressionNotSupported 66
#define MGK_CoderErrorSubsamplingRequiresEvenWidth 67
#define MGK_CoderErrorUnableToCopyProfile 68
#define MGK_CoderErrorUnableToCreateADC 69
#define MGK_CoderErrorUnableToCreateBitmap 70
#define MGK_CoderErrorUnableToDecompressImage 71
#define MGK_CoderErrorUnableToInitializeFPXLibrary 72
#define MGK_CoderErrorUnableToOpenBlob 73
#define MGK_CoderErrorUnableToReadAspectRatio 74
#define MGK_CoderErrorUnableToReadCIELABImages 75
#define MGK_CoderErrorUnableToReadSummaryInfo 76
#define MGK_CoderErrorUnableToSetAffineMatrix 77
#define MGK_CoderErrorUnableToSetAspectRatio 78
#define MGK_CoderErrorUnableToSetColorTwist 79
#define MGK_CoderErrorUnableToSetContrast 80
#define MGK_CoderErrorUnableToSetFilteringValue 81
#define MGK_CoderErrorUnableToSetImageComments 82
#define MGK_CoderErrorUnableToSetImageTitle 83
#define MGK_CoderErrorUnableToSetJPEGLevel 84
#define MGK_CoderErrorUnableToSetRegionOfInterest 85
#define MGK_CoderErrorUnableToSetSummaryInfo 86
#define MGK_CoderErrorUnableToTranslateText 87
#define MGK_CoderErrorUnableToWriteMPEGParameters 88
#define MGK_CoderErrorUnableToWriteTemporaryFile 89
#define MGK_CoderErrorUnableToZipCompressImage 90
#define MGK_CoderErrorUnsupportedBitsPerSample 91
#define MGK_CoderErrorUnsupportedCellTypeInTheMatrix 92
#define MGK_CoderErrorWebPDecodingFailedUserAbort 93
#define MGK_CoderErrorWebPEncodingFailed 94
#define MGK_CoderErrorWebPEncodingFailedBadDimension 95
#define MGK_CoderErrorWebPEncodingFailedBadWrite 96
#define MGK_CoderErrorWebPEncodingFailedBitstreamOutOfMemory 97
#define MGK_CoderErrorWebPEncodingFailedFileTooBig 98
#define MGK_CoderErrorWebPEncodingFailedInvalidConfiguration 99
#define MGK_CoderErrorWebPEncodingFailedNULLParameter 100
#define MGK_CoderErrorWebPEncodingFailedOutOfMemory 101
#define MGK_CoderErrorWebPEncodingFailedPartition0Overflow 102
#define MGK_CoderErrorWebPEncodingFailedPartitionOverflow 103
#define MGK_CoderErrorWebPEncodingFailedUserAbort 104
#define MGK_CoderErrorWebPInvalidConfiguration 105
#define MGK_CoderErrorWebPInvalidParameter 106
#define MGK_CoderErrorZipCompressionNotSupported 107
#define MGK_CoderFatalErrorDefault 108
#define MGK_CoderWarningLosslessToLossyJPEGConversion 109
#define MGK_ConfigureErrorIncludeElementNestedTooDeeply 110
#define MGK_ConfigureErrorRegistryKeyLookupFailed 111
#define MGK_ConfigureErrorStringTokenLengthExceeded 112
#define MGK_ConfigureErrorUnableToAccessConfigureFile 113
#define MGK_ConfigureErrorUnableToAccessFontFile 114
#define MGK_ConfigureErrorUnableToAccessLogFile 115
#define MGK_ConfigureErrorUnableToAccessModuleFile 116
#define MGK_ConfigureFatalErrorDefault 117
#define MGK_ConfigureFatalErrorUnableToChangeToWorkingDirectory 118
#define MGK_ConfigureFatalErrorUnableToGetCurrentDirectory 119
#define MGK_ConfigureFatalErrorUnableToRestoreCurrentDirectory 120
#define MGK_ConfigureWarningDefault 121
#define MGK_CorruptImageErrorAnErrorHasOccurredReadingFromFile 122
#define MGK_CorruptImageErrorAnErrorHasOccurredWritingToFile 123
#define MGK_CorruptImageErrorColormapExceedsColorsLimit 124
#define MGK_CorruptImageErrorCompressionNotValid 125
#define MGK_CorruptImageErrorCorruptImage 126
#define MGK_CorruptImageErrorImageFileDoesNotContainAnyImageData 127
#define MGK_CorruptImageErrorImageFileHasNoScenes 128
#define MGK_CorruptImageErrorImageTypeNotSupported 129
#define MGK_CorruptImageErrorImproperImageHeader 130
#define MGK_CorruptImageErrorInsufficientImageDataInFile 131
#define MGK_CorruptImageErrorInvalidColormapIndex 132
#define MGK_CorruptImageErrorInvalidFileFormatVersion 133
#define MGK_CorruptImageErrorLengthAndFilesizeDoNotMatch 134
#define MGK_CorruptImageErrorMissingImageChannel 135
#define MGK_CorruptImageErrorNegativeOrZeroImageSize 136
#define MGK_CorruptImageErrorNonOS2HeaderSizeError 137
#define MGK_CorruptImageErrorNotEnoughTiles 138
#define MGK_CorruptImageErrorStaticPlanesValueNotEqualToOne 139
#define MGK_CorruptImageErrorSubsamplingRequiresEvenWidth 140
#define MGK_CorruptImageErrorTooMuchImageDataInFile 141
#define MGK_CorruptImageErrorUnableToReadColormapFromDumpFile 142
#define MGK_CorruptImageErrorUnableToReadColorProfile 143
#define MGK_CorruptImageErrorUnableToReadExtensionBlock 144
#define MGK_CorruptImageErrorUnableToReadGenericProfile 145
#define MGK_CorruptImageErrorUnableToReadImageData 146
#define MGK_CorruptImageErrorUnableToReadImageHeader 147
#define MGK_CorruptImageErrorUnableToReadIPTCProfile 148
#define MGK_CorruptImageErrorUnableToReadPixmapFromDumpFile 149
#define MGK_CorruptImageErrorUnableToReadSubImageData 150
#define MGK_CorruptImageErrorUnableToReadVIDImage 151
#define MGK_CorruptImageErrorUnableToReadWindowNameFromDumpFile 152
#define MGK_CorruptImageErrorUnableToRunlengthDecodeImage 153
#define MGK_CorruptImageErrorUnableToUncompressImage 154
#define MGK_CorruptImageErrorUnexpectedEndOfFile 155
#define MGK_CorruptImageErrorUnexpectedSamplingFactor 156
#define MGK_CorruptImageErrorUnknownPatternType 157
#define MGK_CorruptImageErrorUnrecognizedBitsPerPixel 158
#define MGK_CorruptImageErrorUnrecognizedImageCompression 159
#define MGK_CorruptImageErrorUnrecognizedNumberOfColors 160
#define MGK_CorruptImageErrorUnrecognizedXWDHeader 161
#define MGK_CorruptImageErrorUnsupportedBitsPerSample 162
#define MGK_CorruptImageErrorUnsupportedNumberOfPlanes 163
#define MGK_CorruptImageFatalErrorUnableToPersistKey 164
#define MGK_CorruptImageWarningCompressionNotValid 165
#define MGK_CorruptImageWarningCorruptImage 166
#define MGK_CorruptImageWarningImproperImageHeader 167
#define MGK_CorruptImageWarningInvalidColormapIndex 168
#define MGK_CorruptImageWarningLengthAndFilesizeDoNotMatch 169
#define MGK_CorruptImageWarningNegativeOrZeroImageSize 170
#define MGK_CorruptImageWarningNonOS2HeaderSizeError 171
#define MGK_CorruptImageWarningSkipToSyncByte 172
#define MGK_CorruptImageWarningStaticPlanesValueNotEqualToOne 173
#define MGK_CorruptImageWarningUnrecognizedBitsPerPixel 174
#define MGK_CorruptImageWarningUnrecognizedImageCompression 175
#define MGK_DelegateErrorDelegateFailed 176
#define MGK_DelegateErrorFailedToAllocateArgumentList 177
#define MGK_DelegateErrorFailedToAllocateGhostscriptInterpreter 178
#define MGK_DelegateErrorFailedToComputeOutputSize 179
#define MGK_DelegateErrorFailedToFindGhostscript 180
#define MGK_DelegateErrorFailedToRenderFile 181
#define MGK_DelegateErrorFailedToScanFile 182
#define MGK_DelegateErrorNoTagFound 183
#define MGK_DelegateErrorPostscriptDelegateFailed 184
#define MGK_DelegateErrorUnableToCreateImage 185
#define MGK_DelegateErrorUnableToCreateImageComponent 186
#define MGK_DelegateErrorUnableToDecodeImageFile 187
#define MGK_DelegateErrorUnableToEncodeImageFile 188
#define MGK_DelegateErrorUnableToInitializeFPXLibrary 189
#define MGK_DelegateErrorUnableToInitializeWMFLibrary 190
#define MGK_DelegateErrorUnableToManageJP2Stream 191
#define MGK_DelegateErrorUnableToWriteSVGFormat 192
#define MGK_DelegateErrorWebPABIMismatch 193
#define MGK_DelegateFatalErrorDefault 194
#define MGK_DelegateWarningDefault 195
#define MGK_DrawErrorAlreadyPushingPatternDefinition 196
#define MGK_DrawErrorDrawingRecursionDetected 197
#define MGK_DrawErrorFloatValueConversionError 198
#define MGK_DrawErrorIntegerValueConversionError 199
#define MGK_DrawErrorInvalidPrimitiveArgument 200
#define MGK_DrawErrorNonconformingDrawingPrimitiveDefinition 201
#define MGK_DrawErrorPrimitiveArithmeticOverflow 202
#define MGK_DrawErrorTooManyCoordinates 203
#define MGK_DrawErrorUnableToPrint 204
#define MGK_DrawErrorUnbalancedGraphicContextPushPop 205
#define MGK_DrawErrorUnreasonableGradientSize 206
#define MGK_DrawErrorVectorPathTruncated 207
#define MGK_DrawFatalErrorDefault 208
#define MGK_DrawWarningNotARelativeURL 209
#define MGK_DrawWarningNotCurrentlyPushingPatternDefinition 210
#define MGK_DrawWarningURLNotFound 211
#define MGK_FileOpenErrorUnableToCreateTemporaryFile 212
#define MGK_FileOpenErrorUnableToOpenFile 213
#define MGK_FileOpenErrorUnableToWriteFile 214
#define MGK_FileOpenFatalErrorDefault 215
#define MGK_FileOpenWarningDefault 216
#define MGK_ImageErrorAngleIsDiscontinuous 217
#define MGK_ImageErrorCMYKAImageLacksAlphaChannel 218
#define MGK_ImageErrorColorspaceColorProfileMismatch 219
#define MGK_ImageErrorImageColorspaceDiffers 220
#define MGK_ImageErrorImageColorspaceMismatch 221
#define MGK_ImageErrorImageDifferenceExceedsLimit 222
#define MGK_ImageErrorImageDoesNotContainResolution 223
#define MGK_ImageErrorImageIsNotColormapped 224
#define MGK_ImageErrorImageOpacityDiffers 225
#define MGK_ImageErrorImageSequenceIsRequired 226
#define MGK_ImageErrorImageSizeDiffers 227
#define MGK_ImageErrorInvalidColormapIndex 228
#define MGK_ImageErrorLeftAndRightImageSizesDiffer 229
#define MGK_ImageErrorNoImagesWereFound 230
#define MGK_ImageErrorNoImagesWereLoaded 231
#define MGK_ImageErrorNoLocaleImageAttribute 232
#define MGK_ImageErrorTooManyClusters 233
#define MGK_ImageErrorUnableToAppendImage 234
#define MGK_ImageErrorUnableToAssignProfile 235
#define MGK_ImageErrorUnableToAverageImage 236
#define MGK_ImageErrorUnableToCoalesceImage 237
#define MGK_ImageErrorUnableToCompareImages 238
#define MGK_ImageErrorUnableToCreateImageMosaic 239
#define MGK_ImageErrorUnableToCreateStereoImage 240
#define MGK_ImageErrorUnableToDeconstructImageSequence 241
#define MGK_ImageErrorUnableToExportImagePixels 242
#define MGK_ImageErrorUnableToFlattenImage 243
#define MGK_ImageErrorUnableToGetClipMask 244
#define MGK_ImageErrorUnableToGetCompositeMask 245
#define MGK_ImageErrorUnableToHandleImageChannel 246
#define MGK_ImageErrorUnableToImportImagePixels 247
#define MGK_ImageErrorUnableToResizeImage 248
#define MGK_ImageErrorUnableToSegmentImage 249
#define MGK_ImageErrorUnableToSetClipMask 250
#define MGK_ImageErrorUnableToSetCompositeMask 251
#define MGK_ImageErrorUnableToShearImage 252
#define MGK_ImageErrorWidthOrHeightExceedsLimit 253
#define MGK_ImageFatalErrorUnableToPersistKey 254
#define MGK_ImageWarningDefault 255
#define MGK_MissingDelegateErrorDPSLibraryIsNotAvailable 256
#define MGK_MissingDelegateErrorFPXLibraryIsNotAvailable 257
#define MGK_MissingDelegateErrorFreeTypeLibraryIsNotAvailable 258
#define MGK_MissingDelegateErrorJPEGLibraryIsNotAvailable 259
#define MGK_MissingDelegateErrorLCMSLibraryIsNotAvailable 260
#define MGK_MissingDelegateErrorLZWEncodingNotEnabled 261
#define MGK_MissingDelegateErrorNoDecodeDelegateForThisImageFormat 262
#define MGK_MissingDelegateErrorNoEncodeDelegateForThisImageFormat 263
#define MGK_MissingDelegateErrorTIFFLibraryIsNotAvailable 264
#define MGK_MissingDelegateErrorXMLLibraryIsNotAvailable 265
#define MGK_MissingDelegateErrorXWindowLibraryIsNotAvailable 266
#define MGK_MissingDelegateErrorZipLibraryIsNotAvailable 267
#define MGK_MissingDelegateFatalErrorDefault 268
#define MGK_MissingDelegateWarningDefault 269
#define MGK_ModuleErrorFailedToCloseModule 270
#define MGK_ModuleErrorFailedToFindSymbol 271
#define MGK_ModuleErrorUnableToLoadModule 272
#define MGK_ModuleErrorUnableToRegisterImageFormat 273
#define MGK_ModuleErrorUnrecognizedModule 274
#define MGK_ModuleFatalErrorUnableToInitializeModuleLoader 275
#define MGK_ModuleWarningDefault 276
#define MGK_MonitorErrorDefault 277
#define MGK_MonitorFatalErrorDefault 278
#define MGK_MonitorFatalErrorUserRequestedTerminationBySignal 279
#define MGK_MonitorWarningDefault 280
#define MGK_OptionErrorBevelWidthIsNegative 281
#define MGK_OptionErrorColorSeparatedImageRequired 282
#define MGK_OptionErrorFrameIsLessThanImageSize 283
#define MGK_OptionErrorGeometryDimensionsAreZero 284
#define MGK_OptionErrorGeometryDoesNotContainImage 285
#define MGK_OptionErrorHaldClutImageDimensionsInvalid 286
#define MGK_OptionErrorImagesAreNotTheSameSize 287
#define MGK_OptionErrorImageSizeMustExceedBevelWidth 288
#define MGK_OptionErrorImageSmallerThanKernelWidth 289
#define MGK_OptionErrorImageSmallerThanRadius 290
#define MGK_OptionErrorImageWidthsOrHeightsDiffer 291
#define MGK_OptionErrorInputImagesAlreadySpecified 292
#define MGK_OptionErrorInvalidSubimageSpecification 293
#define MGK_OptionErrorKernelRadiusIsTooSmall 294
#define MGK_OptionErrorKernelWidthMustBeAnOddNumber 295
#define MGK_OptionErrorMatrixIsNotSquare 296
#define MGK_OptionErrorMatrixOrderOutOfRange 297
#define MGK_OptionErrorMissingAnImageFilename 298
#define MGK_OptionErrorMissingArgument 299
#define MGK_OptionErrorMustSpecifyAnImageName 300
#define MGK_OptionErrorMustSpecifyImageSize 301
#define MGK_OptionErrorNoBlobDefined 302
#define MGK_OptionErrorNoImagesDefined 303
#define MGK_OptionErrorNonzeroWidthAndHeightRequired 304
#define MGK_OptionErrorNoProfileNameWasGiven 305
#define MGK_OptionErrorNullBlobArgument 306
#define MGK_OptionErrorReferenceImageRequired 307
#define MGK_OptionErrorReferenceIsNotMyType 308
#define MGK_OptionErrorRegionAreaExceedsLimit 309
#define MGK_OptionErrorRequestDidNotReturnAnImage 310
#define MGK_OptionErrorSteganoImageRequired 311
#define MGK_OptionErrorStereoImageRequired 312
#define MGK_OptionErrorSubimageSpecificationReturnsNoImages 313
#define MGK_OptionErrorTileNotBoundedByImageDimensions 314
#define MGK_OptionErrorUnableToAddOrRemoveProfile 315
#define MGK_OptionErrorUnableToAverageImageSequence 316
#define MGK_OptionErrorUnableToBlurImage 317
#define MGK_OptionErrorUnableToChopImage 318
#define MGK_OptionErrorUnableToColorMatrixImage 319
#define MGK_OptionErrorUnableToConstituteImage 320
#define MGK_OptionErrorUnableToConvolveImage 321
#define MGK_OptionErrorUnableToEdgeImage 322
#define MGK_OptionErrorUnableToEqualizeImage 323
#define MGK_OptionErrorUnableToFilterImage 324
#define MGK_OptionErrorUnableToFormatImageMetadata 325
#define MGK_OptionErrorUnableToFrameImage 326
#define MGK_OptionErrorUnableToOilPaintImage 327
#define MGK_OptionErrorUnableToPaintImage 328
#define MGK_OptionErrorUnableToRaiseImage 329
#define MGK_OptionErrorUnableToSharpenImage 330
#define MGK_OptionErrorUnableToThresholdImage 331
#define MGK_OptionErrorUnableToWaveImage 332
#define MGK_OptionErrorUnrecognizedAttribute 333
#define MGK_OptionErrorUnrecognizedChannelType 334
#define MGK_OptionErrorUnrecognizedColor 335
#define MGK_OptionErrorUnrecognizedColormapType 336
#define MGK_OptionErrorUnrecognizedColorspace 337
#define MGK_OptionErrorUnrecognizedCommand 338
#define MGK_OptionErrorUnrecognizedComposeOperator 339
#define MGK_OptionErrorUnrecognizedDisposeMethod 340
#define MGK_OptionErrorUnrecognizedElement 341
#define MGK_OptionErrorUnrecognizedEndianType 342
#define MGK_OptionErrorUnrecognizedGravityType 343
#define MGK_OptionErrorUnrecognizedHighlightStyle 344
#define MGK_OptionErrorUnrecognizedImageCompression 345
#define MGK_OptionErrorUnrecognizedImageFilter 346
#define MGK_OptionErrorUnrecognizedImageFormat 347
#define MGK_OptionErrorUnrecognizedImageMode 348
#define MGK_OptionErrorUnrecognizedImageType 349
#define MGK_OptionErrorUnrecognizedIntentType 350
#define MGK_OptionErrorUnrecognizedInterlaceType 351
#define MGK_OptionErrorUnrecognizedListType 352
#define MGK_OptionErrorUnrecognizedMetric 353
#define MGK_OptionErrorUnrecognizedModeType 354
#define MGK_OptionErrorUnrecognizedNoiseType 355
#define MGK_OptionErrorUnrecognizedOperator 356
#define MGK_OptionErrorUnrecognizedOption 357
#define MGK_OptionErrorUnrecognizedPerlMagickMethod 358
#define MGK_OptionErrorUnrecognizedPixelMap 359
#define MGK_OptionErrorUnrecognizedPreviewType 360
#define MGK_OptionErrorUnrecognizedResourceType 361
#define MGK_OptionErrorUnrecognizedType 362
#define MGK_OptionErrorUnrecognizedUnitsType 363
#define MGK_OptionErrorUnrecognizedVirtualPixelMethod 364
#define MGK_OptionErrorUnsupportedSamplingFactor 365
#define MGK_OptionErrorUsageError 366
#define MGK_OptionFatalErrorInvalidColorspaceType 367
#define MGK_OptionFatalErrorInvalidEndianType 368
#define MGK_OptionFatalErrorInvalidImageType 369
#define MGK_OptionFatalErrorInvalidInterlaceType 370
#define MGK_OptionFatalErrorMissingAnImageFilename 371
#define MGK_OptionFatalErrorMissingArgument 372
#define MGK_OptionFatalErrorNoImagesWereLoaded 373
#define MGK_OptionFatalErrorOptionLengthExceedsLimit 374
#define MGK_OptionFatalErrorRequestDidNotReturnAnImage 375
#define MGK_OptionFatalErrorUnableToOpenXServer 376
#define MGK_OptionFatalErrorUnableToPersistKey 377
#define MGK_OptionFatalErrorUnrecognizedColormapType 378
#define MGK_OptionFatalErrorUnrecognizedColorspaceType 379
#define MGK_OptionFatalErrorUnrecognizedDisposeMethod 380
#define MGK_OptionFatalErrorUnrecognizedEndianType 381
#define MGK_OptionFatalErrorUnrecognizedFilterType 382
#define MGK_OptionFatalErrorUnrecognizedImageCompressionType 383
#define MGK_OptionFatalErrorUnrecognizedImageType 384
#define MGK_OptionFatalErrorUnrecognizedInterlaceType 385
#define MGK_OptionFatalErrorUnrecognizedOption 386
#define MGK_OptionFatalErrorUnrecognizedResourceType 387
#define MGK_OptionFatalErrorUnrecognizedVirtualPixelMethod 388
#define MGK_OptionWarningUnrecognizedColor 389
#define MGK_RegistryErrorImageExpected 390
#define MGK_RegistryErrorImageInfoExpected 391
#define MGK_RegistryErrorStructureSizeMismatch 392
#define MGK_RegistryErrorUnableToGetRegistryID 393
#define MGK_RegistryErrorUnableToLocateImage 394
#define MGK_RegistryErrorUnableToSetRegistry 395
#define MGK_RegistryFatalErrorDefault 396
#define MGK_RegistryWarningDefault 397
#define MGK_ResourceLimitErrorCacheResourcesExhausted 398
#define MGK_ResourceLimitErrorImagePixelHeightLimitExceeded 399
#define MGK_ResourceLimitErrorImagePixelLimitExceeded 400
#define MGK_ResourceLimitErrorImagePixelWidthLimitExceeded 401
#define MGK_ResourceLimitErrorMemoryAllocationFailed 402
#define MGK_ResourceLimitErrorNoPixelsDefinedInCache 403
#define MGK_ResourceLimitErrorPixelCacheAllocationFailed 404
#define MGK_ResourceLimitErrorUnableToAddColorProfile 405
#define MGK_ResourceLimitErrorUnableToAddGenericProfile 406
#define MGK_ResourceLimitErrorUnableToAddIPTCProfile 407
#define MGK_ResourceLimitErrorUnableToAddOrRemoveProfile 408
#define MGK_ResourceLimitErrorUnableToAllocateCoefficients 409
#define MGK_ResourceLimitErrorUnableToAllocateColormap 410
#define MGK_ResourceLimitErrorUnableToAllocateICCProfile 411
#define MGK_ResourceLimitErrorUnableToAllocateImage 412
#define MGK_ResourceLimitErrorUnableToAllocateString 413
#define MGK_ResourceLimitErrorUnableToAnnotateImage 414
#define MGK_ResourceLimitErrorUnableToAverageImageSequence 415
#define MGK_ResourceLimitErrorUnableToCloneDrawingWand 416
#define MGK_ResourceLimitErrorUnableToCloneImage 417
#define MGK_ResourceLimitErrorUnableToComputeImageSignature 418
#define MGK_ResourceLimitErrorUnableToConstituteImage 419
#define MGK_ResourceLimitErrorUnableToConvertFont 420
#define MGK_ResourceLimitErrorUnableToConvertStringToTokens 421
#define MGK_ResourceLimitErrorUnableToCreateColormap 422
#define MGK_ResourceLimitErrorUnableToCreateColorTransform 423
#define MGK_ResourceLimitErrorUnableToCreateCommandWidget 424
#define MGK_ResourceLimitErrorUnableToCreateImageGroup 425
#define MGK_ResourceLimitErrorUnableToCreateImageMontage 426
#define MGK_ResourceLimitErrorUnableToCreateXWindow 427
#define MGK_ResourceLimitErrorUnableToCropImage 428
#define MGK_ResourceLimitErrorUnableToDespeckleImage 429
#define MGK_ResourceLimitErrorUnableToDetermineImageClass 430
#define MGK_ResourceLimitErrorUnableToDetermineTheNumberOfImageColors 431
#define MGK_ResourceLimitErrorUnableToDitherImage 432
#define MGK_ResourceLimitErrorUnableToDrawOnImage 433
#define MGK_ResourceLimitErrorUnableToEdgeImage 434
#define MGK_ResourceLimitErrorUnableToEmbossImage 435
#define MGK_ResourceLimitErrorUnableToEnhanceImage 436
#define MGK_ResourceLimitErrorUnableToFloodfillImage 437
#define MGK_ResourceLimitErrorUnableToGammaCorrectImage 438
#define MGK_ResourceLimitErrorUnableToGetBestIconSize 439
#define MGK_ResourceLimitErrorUnableToGetFromRegistry 440
#define MGK_ResourceLimitErrorUnableToGetPackageInfo 441
#define MGK_ResourceLimitErrorUnableToLevelImage 442
#define MGK_ResourceLimitErrorUnableToMagnifyImage 443
#define MGK_ResourceLimitErrorUnableToManageColor 444
#define MGK_ResourceLimitErrorUnableToMapImage 445
#define MGK_ResourceLimitErrorUnableToMapImageSequence 446
#define MGK_ResourceLimitErrorUnableToMedianFilterImage 447
#define MGK_ResourceLimitErrorUnableToMotionBlurImage 448
#define MGK_ResourceLimitErrorUnableToNoiseFilterImage 449
#define MGK_ResourceLimitErrorUnableToNormalizeImage 450
#define MGK_ResourceLimitErrorUnableToOpenColorProfile 451
#define MGK_ResourceLimitErrorUnableToQuantizeImage 452
#define MGK_ResourceLimitErrorUnableToQuantizeImageSequence 453
#define MGK_ResourceLimitErrorUnableToReadTextChunk 454
#define MGK_ResourceLimitErrorUnableToReadXImage 455
#define MGK_ResourceLimitErrorUnableToReadXServerColormap 456
#define MGK_ResourceLimitErrorUnableToResizeImage 457
#define MGK_ResourceLimitErrorUnableToRotateImage 458
#define MGK_ResourceLimitErrorUnableToSampleImage 459
#define MGK_ResourceLimitErrorUnableToScaleImage 460
#define MGK_ResourceLimitErrorUnableToSelectImage 461
#define MGK_ResourceLimitErrorUnableToSharpenImage 462
#define MGK_ResourceLimitErrorUnableToShaveImage 463
#define MGK_ResourceLimitErrorUnableToShearImage 464
#define MGK_ResourceLimitErrorUnableToSortImageColormap 465
#define MGK_ResourceLimitErrorUnableToThresholdImage 466
#define MGK_ResourceLimitErrorUnableToTransformColorspace 467
#define MGK_ResourceLimitFatalErrorMemoryAllocationFailed 468
#define MGK_ResourceLimitFatalErrorSemaporeOperationFailed 469
#define MGK_ResourceLimitFatalErrorUnableToAllocateAscii85Info 470
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheInfo 471
#define MGK_ResourceLimitFatalErrorUnableToAllocateCacheView 472
#define MGK_ResourceLimitFatalErrorUnableToAllocateColorInfo 473
#define MGK_ResourceLimitFatalErrorUnableToAllocateDashPattern 474
#define MGK_ResourceLimitFatalErrorUnableToAllocateDelegateInfo 475
#define MGK_ResourceLimitFatalErrorUnableToAllocateDerivatives 476
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawContext 477
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawInfo 478
#define MGK_ResourceLimitFatalErrorUnableToAllocateDrawingWand 479
#define MGK_ResourceLimitFatalErrorUnableToAllocateGammaMap 480
#define MGK_ResourceLimitFatalErrorUnableToAllocateImage 481
#define MGK_ResourceLimitFatalErrorUnableToAllocateImagePixels 482
#define MGK_ResourceLimitFatalErrorUnableToAllocateLogInfo 483
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagicInfo 484
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickInfo 485
#define MGK_ResourceLimitFatalErrorUnableToAllocateMagickMap 486
#define MGK_ResourceLimitFatalErrorUnableToAllocateModuleInfo 487
#define MGK_ResourceLimitFatalErrorUnableToAllocateMontageInfo 488
#define MGK_ResourceLimitFatalErrorUnableToAllocateQuantizeInfo 489
#define MGK_ResourceLimitFatalErrorUnableToAllocateRandomKernel 490
#define MGK_ResourceLimitFatalErrorUnableToAllocateRegistryInfo 491
#define MGK_ResourceLimitFatalErrorUnableToAllocateSemaphoreInfo 492
#define MGK_ResourceLimitFatalErrorUnableToAllocateString 493
#define MGK_ResourceLimitFatalErrorUnableToAllocateTypeInfo 494
#define MGK_ResourceLimitFatalErrorUnableToAllocateWand 495
#define MGK_ResourceLimitFatalErrorUnableToAnimateImageSequence 496
#define MGK_ResourceLimitFatalErrorUnableToCloneBlobInfo 497
#define MGK_ResourceLimitFatalErrorUnableToCloneCacheInfo 498
#define MGK_ResourceLimitFatalErrorUnableToCloneImage 499
#define MGK_ResourceLimitFatalErrorUnableToCloneImageInfo 500
#define MGK_ResourceLimitFatalErrorUnableToConcatenateString 501
#define MGK_ResourceLimitFatalErrorUnableToConvertText 502
#define MGK_ResourceLimitFatalErrorUnableToCreateColormap 503
#define MGK_ResourceLimitFatalErrorUnableToDestroySemaphore 504
#define MGK_ResourceLimitFatalErrorUnableToDisplayImage 505
#define MGK_ResourceLimitFatalErrorUnableToEscapeString 506
#define MGK_ResourceLimitFatalErrorUnableToInitializeSemaphore 507
#define MGK_ResourceLimitFatalErrorUnableToInterpretMSLImage 508
#define MGK_ResourceLimitFatalErrorUnableToLockSemaphore 509
#define MGK_ResourceLimitFatalErrorUnableToObtainRandomEntropy 510
#define MGK_ResourceLimitFatalErrorUnableToUnlockSemaphore 511
#define MGK_ResourceLimitWarningMemoryAllocationFailed 512
#define MGK_StreamErrorImageDoesNotContainTheStreamGeometry 513
#define MGK_StreamErrorNoStreamHandlerIsDefined 514
#define MGK_StreamErrorPixelCacheIsNotOpen 515
#define MGK_StreamErrorUnableToAcquirePixelStream 516
#define MGK_StreamErrorUnableToSetPixelStream 517
#define MGK_StreamErrorUnableToSyncPixelStream 518
#define MGK_StreamFatalErrorDefault 519
#define MGK_StreamWarningDefault 520
#define MGK_TypeErrorFontSubstitutionRequired 521
#define MGK_TypeErrorUnableToGetTypeMetrics 522
#define MGK_TypeErrorUnableToInitializeFreetypeLibrary 523
#define MGK_TypeErrorUnableToReadFont 524
#define MGK_TypeErrorUnrecognizedFontEncoding 525
#define MGK_TypeFatalErrorDefault 526
#define MGK_TypeWarningDefault 527
#define MGK_WandErrorInvalidColormapIndex 528
#define MGK_WandErrorWandAPINotImplemented 529
#define MGK_WandErrorWandContainsNoImageIndexs 530
#define MGK_WandErrorWandContainsNoImages 531
#define MGK_XServerErrorColorIsNotKnownToServer 532
#define MGK_XServerErrorNoWindowWithSpecifiedIDExists 533
#define MGK_XServerErrorStandardColormapIsNotInitialized 534
#define MGK_XServerErrorUnableToConnectToRemoteDisplay 535
#define MGK_XServerErrorUnableToCreateBitmap 536
#define MGK_XServerErrorUnableToCreateColormap 537
#define MGK_XServerErrorUnableToCreatePixmap 538
#define MGK_XServerErrorUnableToCreateProperty 539
#define MGK_XServerErrorUnableToCreateStandardColormap 540
#define MGK_XServerErrorUnableToDisplayImageInfo 541
#define MGK_XServerErrorUnableToGetProperty 542
#define MGK_XServerErrorUnableToGetStandardColormap 543
#define MGK_XServerErrorUnableToGetVisual 544
#define MGK_XServerErrorUnableToGrabMouse 545
#define MGK_XServerErrorUnableToLoadFont 546
#define MGK_XServerErrorUnableToMatchVisualToStandardColormap 547
#define MGK_XServerErrorUnableToOpenXServer 548
#define MGK_XServerErrorUnableToReadXAttributes 549
#define MGK_XServerErrorUnableToReadXWindowImage 550
#define MGK_XServerErrorUnrecognizedColormapType 551
#define MGK_XServerErrorUnrecognizedGravityType 552
#define MGK_XServerErrorUnrecognizedVisualSpecifier 553
#define MGK_XServerFatalErrorUnableToAllocateXHints 554
#define MGK_XServerFatalErrorUnableToCreateCursor 555
#define MGK_XServerFatalErrorUnableToCreateGraphicContext 556
#define MGK_XServerFatalErrorUnableToCreateStandardColormap 557
#define MGK_XServerFatalErrorUnableToCreateTextProperty 558
#define MGK_XServerFatalErrorUnableToCreateXImage 559
#define MGK_XServerFatalErrorUnableToCreateXPixmap 560
#define MGK_XServerFatalErrorUnableToCreateXWindow 561
#define MGK_XServerFatalErrorUnableToDisplayImage 562
#define MGK_XServerFatalErrorUnableToDitherImage 563
#define MGK_XServerFatalErrorUnableToGetPixelInfo 564
#define MGK_XServerFatalErrorUnableToGetVisual 565
#define MGK_XServerFatalErrorUnableToLoadFont 566
#define MGK_XServerFatalErrorUnableToMakeXWindow 567
#define MGK_XServerFatalErrorUnableToOpenXServer 568
#define MGK_XServerFatalErrorUnableToViewFonts 569
#define MGK_XServerWarningUnableToGetVisual 570
#define MGK_XServerWarningUsingDefaultVisual 571

#endif

#if defined(_INCLUDE_CATEGORYMAP_TABLE_)
typedef struct _CategoryInfo{
  const char *name;
  int offset;
} CategoryInfo;

static const CategoryInfo category_map[] =
  {
    { "Blob", 0 },
    { "Cache", 3 },
    { "Coder", 6 },
    { "Configure", 9 },
    { "Corrupt/Image", 12 },
    { "Delegate", 15 },
    { "Draw", 18 },
    { "File/Open", 21 },
    { "Image", 24 },
    { "Missing/Delegate", 27 },
    { "Module", 30 },
    { "Monitor", 33 },
    { "Option", 36 },
    { "Registry", 39 },
    { "Resource/Limit", 42 },
    { "Stream", 45 },
    { "Type", 48 },
    { "Wand", 51 },
    { "XServer", 52 },
    { 0, 54 }
  };
#endif

#if defined(_INCLUDE_SEVERITYMAP_TABLE_)
typedef struct _SeverityInfo{
  const char *name;
  int offset;
  ExceptionType severityid;
} SeverityInfo;

static const SeverityInfo severity_map[] =
  {
    { "Blob/Error", 0, BlobError },
    { "Blob/FatalError", 9, BlobFatalError },
    { "Blob/Warning", 10, BlobWarning },
    { "Cache/Error", 11, CacheError },
    { "Cache/FatalError", 23, CacheFatalError },
    { "Cache/Warning", 25, CacheWarning },
    { "Coder/Error", 26, CoderError },
    { "Coder/FatalError", 107, CoderFatalError },
    { "Coder/Warning", 108, CoderWarning },
    { "Configure/Error", 109, ConfigureError },
    { "Configure/FatalError", 116, ConfigureFatalError },
    { "Configure/Warning", 120, ConfigureWarning },
    { "Corrupt/Image/Error", 121, CorruptImageError },
    { "Corrupt/Image/FatalError", 163, CorruptImageFatalError },
    { "Corrupt/Image/Warning", 164, CorruptImageWarning },
    { "Delegate/Error", 175, DelegateError },
    { "Delegate/FatalError", 193, DelegateFatalError },
    { "Delegate/Warning", 194, DelegateWarning },
    { "Draw/Error", 195, DrawError },
    { "Draw/FatalError", 207, DrawFatalError },
    { "Draw/Warning", 208, DrawWarning },
    { "File/Open/Error", 211, FileOpenError },
    { "File/Open/FatalError", 214, FileOpenFatalError },
    { "File/Open/Warning", 215, FileOpenWarning },
    { "Image/Error", 216, ImageError },
    { "Image/FatalError", 253, ImageFatalError },
    { "Image/Warning", 254, ImageWarning },
    { "Missing/Delegate/Error", 255, MissingDelegateError },
    { "Missing/Delegate/FatalError", 267, MissingDelegateFatalError },
    { "Missing/Delegate/Warning", 268, MissingDelegateWarning },
    { "Module/Error", 269, ModuleError },
    { "Module/FatalError", 274, ModuleFatalError },
    { "Module/Warning", 275, ModuleWarning },
    { "Monitor/Error", 276, MonitorError },
    { "Monitor/FatalError", 277, MonitorFatalError },
    { "Monitor/Warning", 279, MonitorWarning },
    { "Option/Error", 280, OptionError },
    { "Option/FatalError", 366, OptionFatalError },
    { "Option/Warning", 388, OptionWarning },
    { "Registry/Error", 389, RegistryError },
    { "Registry/FatalError", 395, RegistryFatalError },
    { "Registry/Warning", 396, RegistryWarning },
    { "Resource/Limit/Error", 397, ResourceLimitError },
    { "Resource/Limit/FatalError", 467, ResourceLimitFatalError },
    { "Resource/Limit/Warning", 511, ResourceLimitWarning },
    { "Stream/Error", 512, StreamError },
    { "Stream/FatalError", 518, StreamFatalError },
    { "Stream/Warning", 519, StreamWarning },
    { "Type/Error", 520, TypeError },
    { "Type/FatalError", 525, TypeFatalError },
    { "Type/Warning", 526, TypeWarning },
    { "Wand/Error", 527, WandError },
    { "XServer/Error", 531, XServerError },
    { "XServer/FatalError", 553, XServerFatalError },
    { "XServer/Warning", 569, XServerWarning },
    { 0, 571, UndefinedException }
  };
#endif

#if defined(_INCLUDE_TAGMAP_TABLE_)
typedef struct _MessageInfo
{
  const char *name;
  int messageid;
} MessageInfo;

static const MessageInfo message_map[] =
  {
    { "UnableToCreateBlob", 1 },
    { "UnableToDeduceImageFormat", 2 },
    { "UnableToObtainOffset", 3 },
    { "UnableToOpenFile", 4 },
    { "UnableToReadFile", 5 },
    { "UnableToReadToOffset", 6 },
    { "UnableToSeekToOffset", 7 },
    { "UnableToWriteBlob", 8 },
    { "UnrecognizedImageFormat", 9 },
    { "Default", 10 },
    { "Default", 11 },
    { "InconsistentPersistentCacheDepth", 12 },
    { "PixelCacheDimensionsMisMatch", 13 },
    { "PixelCacheIsNotOpen", 14 },
    { "UnableToAllocateCacheView", 15 },
    { "UnableToCloneCache", 16 },
    { "UnableToExtendCache", 17 },
    { "UnableToGetCacheNexus", 18 },
    { "UnableToGetPixelsFromCache", 19 },
    { "UnableToOpenCache", 20 },
    { "UnableToPeristPixelCache", 21 },
    { "UnableToReadPixelCache", 22 },
    { "UnableToSyncCache", 23 },
    { "DiskAllocationFailed", 24 },
    { "UnableToExtendPixelCache", 25 },
    { "Default", 26 },
    { "ArithmeticOverflow", 27 },
    { "ColormapTooLarge", 28 },
    { "ColormapTypeNotSupported", 29 },
    { "ColorspaceModelIsNotSupported", 30 },
    { "ColorTypeNotSupported", 31 },
    { "CompressionNotValid", 32 },
    { "DataEncodingSchemeIsNotSupported", 33 },
    { "DataStorageTypeIsNotSupported", 34 },
    { "DeltaPNGNotSupported", 35 },
    { "DivisionByZero", 36 },
    { "EncryptedWPGImageFileNotSupported", 37 },
    { "FractalCompressionNotSupported", 38 },
    { "ImageColumnOrRowSizeIsNotSupported", 39 },
    { "ImageDoesNotHaveAMatteChannel", 40 },
    { "ImageIsNotTiled", 41 },
    { "ImageTypeNotSupported", 42 },
    { "IncompatibleSizeOfDouble", 43 },
    { "IrregularChannelGeometryNotSupported", 44 },
    { "JNGCompressionNotSupported", 45 },
    { "JPEGCompressionNotSupported", 46 },
    { "JPEGEmbeddingFailed", 47 },
    { "LocationTypeIsNotSupported", 48 },
    { "MapStorageTypeIsNotSupported", 49 },
    { "MSBByteOrderNotSupported", 50 },
    { "MultidimensionalMatricesAreNotSupported", 51 },
    { "MultipleRecordListNotSupported", 52 },
    { "No8BIMDataIsAvailable", 53 },
    { "NoAPP1DataIsAvailable", 54 },
    { "NoBitmapOnClipboard", 55 },
    { "NoColorProfileAvailable", 56 },
    { "NoDataReturned", 57 },
    { "NoImageVectorGraphics", 58 },
    { "NoIPTCInfoWasFound", 59 },
    { "NoIPTCProfileAvailable", 60 },
    { "NumberOfImagesIsNotSupported", 61 },
    { "OnlyContinuousTonePictureSupported", 62 },
    { "OnlyLevelZerofilesSupported", 63 },
    { "PNGCompressionNotSupported", 64 },
    { "PNGLibraryTooOld", 65 },
    { "RLECompressionNotSupported", 66 },
    { "SubsamplingRequiresEvenWidth", 67 },
    { "UnableToCopyProfile", 68 },
    { "UnableToCreateADC", 69 },
    { "UnableToCreateBitmap", 70 },
    { "UnableToDecompressImage", 71 },
    { "UnableToInitializeFPXLibrary", 72 },
    { "UnableToOpenBlob", 73 },
    { "UnableToReadAspectRatio", 74 },
    { "UnableToReadCIELABImages", 75 },
    { "UnableToReadSummaryInfo", 76 },
    { "UnableToSetAffineMatrix", 77 },
    { "UnableToSetAspectRatio", 78 },
    { "UnableToSetColorTwist", 79 },
    { "UnableToSetContrast", 80 },
    { "UnableToSetFilteringValue", 81 },
    { "UnableToSetImageComments", 82 },
    { "UnableToSetImageTitle", 83 },
    { "UnableToSetJPEGLevel", 84 },
    { "UnableToSetRegionOfInterest", 85 },
    { "UnableToSetSummaryInfo", 86 },
    { "UnableToTranslateText", 87 },
    { "UnableToWriteMPEGParameters", 88 },
    { "UnableToWriteTemporaryFile", 89 },
    { "UnableToZipCompressImage", 90 },
    { "UnsupportedBitsPerSample", 91 },
    { "UnsupportedCellTypeInTheMatrix", 92 },
    { "WebPDecodingFailedUserAbort", 93 },
    { "WebPEncodingFailed", 94 },
    { "WebPEncodingFailedBadDimension", 95 },
    { "WebPEncodingFailedBadWrite", 96 },
    { "WebPEncodingFailedBitstreamOutOfMemory", 97 },
    { "WebPEncodingFailedFileTooBig", 98 },
    { "WebPEncodingFailedInvalidConfiguration", 99 },
    { "WebPEncodingFailedNULLParameter", 100 },
    { "WebPEncodingFailedOutOfMemory", 101 },
    { "WebPEncodingFailedPartition0Overflow", 102 },
    { "WebPEncodingFailedPartitionOverflow", 103 },
    { "WebPEncodingFailedUserAbort", 104 },
    { "WebPInvalidConfiguration", 105 },
    { "WebPInvalidParameter", 106 },
    { "ZipCompressionNotSupported", 107 },
    { "Default", 108 },
    { "LosslessToLossyJPEGConversion", 109 },
    { "IncludeElementNestedTooDeeply", 110 },
    { "RegistryKeyLookupFailed", 111 },
    { "StringTokenLengthExceeded", 112 },
    { "UnableToAccessConfigureFile", 113 },
    { "UnableToAccessFontFile", 114 },
    { "UnableToAccessLogFile", 115 },
    { "UnableToAccessModuleFile", 116 },
    { "Default", 117 },
    { "UnableToChangeToWorkingDirectory", 118 },
    { "UnableToGetCurrentDirectory", 119 },
    { "UnableToRestoreCurrentDirectory", 120 },
    { "Default", 121 },
    { "AnErrorHasOccurredReadingFromFile", 122 },
    { "AnErrorHasOccurredWritingToFile", 123 },
    { "ColormapExceedsColorsLimit", 124 },
    { "CompressionNotValid", 125 },
    { "CorruptImage", 126 },
    { "ImageFileDoesNotContainAnyImageData", 127 },
    { "ImageFileHasNoScenes", 128 },
    { "ImageTypeNotSupported", 129 },
    { "ImproperImageHeader", 130 },
    { "InsufficientImageDataInFile", 131 },
    { "InvalidColormapIndex", 132 },
    { "InvalidFileFormatVersion", 133 },
    { "LengthAndFilesizeDoNotMatch", 134 },
    { "MissingImageChannel", 135 },
    { "NegativeOrZeroImageSize", 136 },
    { "NonOS2HeaderSizeError", 137 },
    { "NotEnoughTiles", 138 },
    { "StaticPlanesValueNotEqualToOne", 139 },
    { "SubsamplingRequiresEvenWidth", 140 },
    { "TooMuchImageDataInFile", 141 },
    { "UnableToReadColormapFromDumpFile", 142 },
    { "UnableToReadColorProfile", 143 },
    { "UnableToReadExtensionBlock", 144 },
    { "UnableToReadGenericProfile", 145 },
    { "UnableToReadImageData", 146 },
    { "UnableToReadImageHeader", 147 },
    { "UnableToReadIPTCProfile", 148 },
    { "UnableToReadPixmapFromDumpFile", 149 },
    { "UnableToReadSubImageData", 150 },
    { "UnableToReadVIDImage", 151 },
    { "UnableToReadWindowNameFromDumpFile", 152 },
    { "UnableToRunlengthDecodeImage", 153 },
    { "UnableToUncompressImage", 154 },
    { "UnexpectedEndOfFile", 155 },
    { "UnexpectedSamplingFactor", 156 },
    { "UnknownPatternType", 157 },
    { "UnrecognizedBitsPerPixel", 158 },
    { "UnrecognizedImageCompression", 159 },
    { "UnrecognizedNumberOfColors", 160 },
    { "UnrecognizedXWDHeader", 161 },
    { "UnsupportedBitsPerSample", 162 },
    { "UnsupportedNumberOfPlanes", 163 },
    { "UnableToPersistKey", 164 },
    { "CompressionNotValid", 165 },
    { "CorruptImage", 166 },
    { "ImproperImageHeader", 167 },
    { "InvalidColormapIndex", 168 },
    { "LengthAndFilesizeDoNotMatch", 169 },
    { "NegativeOrZeroImageSize", 170 },
    { "NonOS2HeaderSizeError", 171 },
    { "SkipToSyncByte", 172 },
    { "StaticPlanesValueNotEqualToOne", 173 },
    { "UnrecognizedBitsPerPixel", 174 },
    { "UnrecognizedImageCompression", 175 },
    { "DelegateFailed", 176 },
    { "FailedToAllocateArgumentList", 177 },
    { "FailedToAllocateGhostscriptInterpreter", 178 },
    { "FailedToComputeOutputSize", 179 },
    { "FailedToFindGhostscript", 180 },
    { "FailedToRenderFile", 181 },
    { "FailedToScanFile", 182 },
    { "NoTagFound", 183 },
    { "PostscriptDelegateFailed", 184 },
    { "UnableToCreateImage", 185 },
    { "UnableToCreateImageComponent", 186 },
    { "UnableToDecodeImageFile", 187 },
    { "UnableToEncodeImageFile", 188 },
    { "UnableToInitializeFPXLibrary", 189 },
    { "UnableToInitializeWMFLibrary", 190 },
    { "UnableToManageJP2Stream", 191 },
    { "UnableToWriteSVGFormat", 192 },
    { "WebPABIMismatch", 193 },
    { "Default", 194 },
    { "Default", 195 },
    { "AlreadyPushingPatternDefinition", 196 },
    { "DrawingRecursionDetected", 197 },
    { "FloatValueConversionError", 198 },
    { "IntegerValueConversionError", 199 },
    { "InvalidPrimitiveArgument", 200 },
    { "NonconformingDrawingPrimitiveDefinition", 201 },
    { "PrimitiveArithmeticOverflow", 202 },
    { "TooManyCoordinates", 203 },
    { "UnableToPrint", 204 },
    { "UnbalancedGraphicContextPushPop", 205 },
    { "UnreasonableGradientSize", 206 },
    { "VectorPathTruncated", 207 },
    { "Default", 208 },
    { "NotARelativeURL", 209 },
    { "NotCurrentlyPushingPatternDefinition", 210 },
    { "URLNotFound", 211 },
    { "UnableToCreateTemporaryFile", 212 },
    { "UnableToOpenFile", 213 },
    { "UnableToWriteFile", 214 },
    { "Default", 215 },
    { "Default", 216 },
    { "AngleIsDiscontinuous", 217 },
    { "CMYKAImageLacksAlphaChannel", 218 },
    { "ColorspaceColorProfileMismatch", 219 },
    { "ImageColorspaceDiffers", 220 },
    { "ImageColorspaceMismatch", 221 },
    { "ImageDifferenceExceedsLimit", 222 },
    { "ImageDoesNotContainResolution", 223 },
    { "ImageIsNotColormapped", 224 },
    { "ImageOpacityDiffers", 225 },
    { "ImageSequenceIsRequired", 226 },
    { "ImageSizeDiffers", 227 },
    { "InvalidColormapIndex", 228 },
    { "LeftAndRightImageSizesDiffer", 229 },
    { "NoImagesWereFound", 230 },
    { "NoImagesWereLoaded", 231 },
    { "NoLocaleImageAttribute", 232 },
    { "TooManyClusters", 233 },
    { "UnableToAppendImage", 234 },
    { "UnableToAssignProfile", 235 },
    { "UnableToAverageImage", 236 },
    { "UnableToCoalesceImage", 237 },
    { "UnableToCompareImages", 238 },
    { "UnableToCreateImageMosaic", 239 },
    { "UnableToCreateStereoImage", 240 },
    { "UnableToDeconstructImageSequence", 241 },
    { "UnableToExportImagePixels", 242 },
    { "UnableToFlattenImage", 243 },
    { "UnableToGetClipMask", 244 },
    { "UnableToGetCompositeMask", 245 },
    { "UnableToHandleImageChannel", 246 },
    { "UnableToImportImagePixels", 247 },
    { "UnableToResizeImage", 248 },
    { "UnableToSegmentImage", 249 },
    { "UnableToSetClipMask", 250 },
    { "UnableToSetCompositeMask", 251 },
    { "UnableToShearImage", 252 },
    { "WidthOrHeightExceedsLimit", 253 },
    { "UnableToPersistKey", 254 },
    { "Default", 255 },
    { "DPSLibraryIsNotAvailable", 256 },
    { "FPXLibraryIsNotAvailable", 257 },
    { "FreeTypeLibraryIsNotAvailable", 258 },
    { "JPEGLibraryIsNotAvailable", 259 },
    { "LCMSLibraryIsNotAvailable", 260 },
    { "LZWEncodingNotEnabled", 261 },
    { "NoDecodeDelegateForThisImageFormat", 262 },
    { "NoEncodeDelegateForThisImageFormat", 263 },
    { "TIFFLibraryIsNotAvailable", 264 },
    { "XMLLibraryIsNotAvailable", 265 },
    { "XWindowLibraryIsNotAvailable", 266 },
    { "ZipLibraryIsNotAvailable", 267 },
    { "Default", 268 },
    { "Default", 269 },
    { "FailedToCloseModule", 270 },
    { "FailedToFindSymbol", 271 },
    { "UnableToLoadModule", 272 },
    { "UnableToRegisterImageFormat", 273 },
    { "UnrecognizedModule", 274 },
    { "UnableToInitializeModuleLoader", 275 },
    { "Default", 276 },
    { "Default", 277 },
    { "Default", 278 },
    { "UserRequestedTerminationBySignal", 279 },
    { "Default", 280 },
    { "BevelWidthIsNegative", 281 },
    { "ColorSeparatedImageRequired", 282 },
    { "FrameIsLessThanImageSize", 283 },
    { "GeometryDimensionsAreZero", 284 },
    { "GeometryDoesNotContainImage", 285 },
    { "HaldClutImageDimensionsInvalid", 286 },
    { "ImagesAreNotTheSameSize", 287 },
    { "ImageSizeMustExceedBevelWidth", 288 },
    { "ImageSmallerThanKernelWidth", 289 },
    { "ImageSmallerThanRadius", 290 },
    { "ImageWidthsOrHeightsDiffer", 291 },
    { "InputImagesAlreadySpecified", 292 },
    { "InvalidSubimageSpecification", 293 },
    { "KernelRadiusIsTooSmall", 294 },
    { "KernelWidthMustBeAnOddNumber", 295 },
    { "MatrixIsNotSquare", 296 },
    { "MatrixOrderOutOfRange", 297 },
    { "MissingAnImageFilename", 298 },
    { "MissingArgument", 299 },
    { "MustSpecifyAnImageName", 300 },
    { "MustSpecifyImageSize", 301 },
    { "NoBlobDefined", 302 },
    { "NoImagesDefined", 303 },
    { "NonzeroWidthAndHeightRequired", 304 },
    { "NoProfileNameWasGiven", 305 },
    { "NullBlobArgument", 306 },
    { "ReferenceImageRequired", 307 },
    { "ReferenceIsNotMyType", 308 },
    { "RegionAreaExceedsLimit", 309 },
    { "RequestDidNotReturnAnImage", 310 },
    { "SteganoImageRequired", 311 },
    { "StereoImageRequired", 312 },
    { "SubimageSpecificationReturnsNoImages", 313 },
    { "TileNotBoundedByImageDimensions", 314 },
    { "UnableToAddOrRemoveProfile", 315 },
    { "UnableToAverageImageSequence", 316 },
    { "UnableToBlurImage", 317 },
    { "UnableToChopImage", 318 },
    { "UnableToColorMatrixImage", 319 },
    { "UnableToConstituteImage", 320 },
    { "UnableToConvolveImage", 321 },
    { "UnableToEdgeImage", 322 },
    { "UnableToEqualizeImage", 323 },
    { "UnableToFilterImage", 324 },
    { "UnableToFormatImageMetadata", 325 },
    { "UnableToFrameImage", 326 },
    { "UnableToOilPaintImage", 327 },
    { "UnableToPaintImage", 328 },
    { "UnableToRaiseImage", 329 },
    { "UnableToSharpenImage", 330 },
    { "UnableToThresholdImage", 331 },
    { "UnableToWaveImage", 332 },
    { "UnrecognizedAttribute", 333 },
    { "UnrecognizedChannelType", 334 },
    { "UnrecognizedColor", 335 },
    { "UnrecognizedColormapType", 336 },
    { "UnrecognizedColorspace", 337 },
    { "UnrecognizedCommand", 338 },
    { "UnrecognizedComposeOperator", 339 },
    { "UnrecognizedDisposeMethod", 340 },
    { "UnrecognizedElement", 341 },
    { "UnrecognizedEndianType", 342 },
    { "UnrecognizedGravityType", 343 },
    { "UnrecognizedHighlightStyle", 344 },
    { "UnrecognizedImageCompression", 345 },
    { "UnrecognizedImageFilter", 346 },
    { "UnrecognizedImageFormat", 347 },
    { "UnrecognizedImageMode", 348 },
    { "UnrecognizedImageType", 349 },
    { "UnrecognizedIntentType", 350 },
    { "UnrecognizedInterlaceType", 351 },
    { "UnrecognizedListType", 352 },
    { "UnrecognizedMetric", 353 },
    { "UnrecognizedModeType", 354 },
    { "UnrecognizedNoiseType", 355 },
    { "UnrecognizedOperator", 356 },
    { "UnrecognizedOption", 357 },
    { "UnrecognizedPerlMagickMethod", 358 },
    { "UnrecognizedPixelMap", 359 },
    { "UnrecognizedPreviewType", 360 },
    { "UnrecognizedResourceType", 361 },
    { "UnrecognizedType", 362 },
    { "UnrecognizedUnitsType", 363 },
    { "UnrecognizedVirtualPixelMethod", 364 },
    { "UnsupportedSamplingFactor", 365 },
    { "UsageError", 366 },
    { "InvalidColorspaceType", 367 },
    { "InvalidEndianType", 368 },
    { "InvalidImageType", 369 },
    { "InvalidInterlaceType", 370 },
    { "MissingAnImageFilename", 371 },
    { "MissingArgument", 372 },
    { "NoImagesWereLoaded", 373 },
    { "OptionLengthExceedsLimit", 374 },
    { "RequestDidNotReturnAnImage", 375 },
    { "UnableToOpenXServer", 376 },
    { "UnableToPersistKey", 377 },
    { "UnrecognizedColormapType", 378 },
    { "UnrecognizedColorspaceType", 379 },
    { "UnrecognizedDisposeMethod", 380 },
    { "UnrecognizedEndianType", 381 },
    { "UnrecognizedFilterType", 382 },
    { "UnrecognizedImageCompressionType", 383 },
    { "UnrecognizedImageType", 384 },
    { "UnrecognizedInterlaceType", 385 },
    { "UnrecognizedOption", 386 },
    { "UnrecognizedResourceType", 387 },
    { "UnrecognizedVirtualPixelMethod", 388 },
    { "UnrecognizedColor", 389 },
    { "ImageExpected", 390 },
    { "ImageInfoExpected", 391 },
    { "StructureSizeMismatch", 392 },
    { "UnableToGetRegistryID", 393 },
    { "UnableToLocateImage", 394 },
    { "UnableToSetRegistry", 395 },
    { "Default", 396 },
    { "Default", 397 },
    { "CacheResourcesExhausted", 398 },
    { "ImagePixelHeightLimitExceeded", 399 },
    { "ImagePixelLimitExceeded", 400 },
    { "ImagePixelWidthLimitExceeded", 401 },
    { "MemoryAllocationFailed", 402 },
    { "NoPixelsDefinedInCache", 403 },
    { "PixelCacheAllocationFailed", 404 },
    { "UnableToAddColorProfile", 405 },
    { "UnableToAddGenericProfile", 406 },
    { "UnableToAddIPTCProfile", 407 },
    { "UnableToAddOrRemoveProfile", 408 },
    { "UnableToAllocateCoefficients", 409 },
    { "UnableToAllocateColormap", 410 },
    { "UnableToAllocateICCProfile", 411 },
    { "UnableToAllocateImage", 412 },
    { "UnableToAllocateString", 413 },
    { "UnableToAnnotateImage", 414 },
    { "UnableToAverageImageSequence", 415 },
    { "UnableToCloneDrawingWand", 416 },
    { "UnableToCloneImage", 417 },
    { "UnableToComputeImageSignature", 418 },
    { "UnableToConstituteImage", 419 },
    { "UnableToConvertFont", 420 },
    { "UnableToConvertStringToTokens", 421 },
    { "UnableToCreateColormap", 422 },
    { "UnableToCreateColorTransform", 423 },
    { "UnableToCreateCommandWidget", 424 },
    { "UnableToCreateImageGroup", 425 },
    { "UnableToCreateImageMontage", 426 },
    { "UnableToCreateXWindow", 427 },
    { "UnableToCropImage", 428 },
    { "UnableToDespeckleImage", 429 },
    { "UnableToDetermineImageClass", 430 },
    { "UnableToDetermineTheNumberOfImageColors", 431 },
    { "UnableToDitherImage", 432 },
    { "UnableToDrawOnImage", 433 },
    { "UnableToEdgeImage", 434 },
    { "UnableToEmbossImage", 435 },
    { "UnableToEnhanceImage", 436 },
    { "UnableToFloodfillImage", 437 },
    { "UnableToGammaCorrectImage", 438 },
    { "UnableToGetBestIconSize", 439 },
    { "UnableToGetFromRegistry", 440 },
    { "UnableToGetPackageInfo", 441 },
    { "UnableToLevelImage", 442 },
    { "UnableToMagnifyImage", 443 },
    { "UnableToManageColor", 444 },
    { "UnableToMapImage", 445 },
    { "UnableToMapImageSequence", 446 },
    { "UnableToMedianFilterImage", 447 },
    { "UnableToMotionBlurImage", 448 },
    { "UnableToNoiseFilterImage", 449 },
    { "UnableToNormalizeImage", 450 },
    { "UnableToOpenColorProfile", 451 },
    { "UnableToQuantizeImage", 452 },
    { "UnableToQuantizeImageSequence", 453 },
    { "UnableToReadTextChunk", 454 },
    { "UnableToReadXImage", 455 },
    { "UnableToReadXServerColormap", 456 },
    { "UnableToResizeImage", 457 },
    { "UnableToRotateImage", 458 },
    { "UnableToSampleImage", 459 },
    { "UnableToScaleImage", 460 },
    { "UnableToSelectImage", 461 },
    { "UnableToSharpenImage", 462 },
    { "UnableToShaveImage", 463 },
    { "UnableToShearImage", 464 },
    { "UnableToSortImageColormap", 465 },
    { "UnableToThresholdImage", 466 },
    { "UnableToTransformColorspace", 467 },
    { "MemoryAllocationFailed", 468 },
    { "SemaporeOperationFailed", 469 },
    { "UnableToAllocateAscii85Info", 470 },
    { "UnableToAllocateCacheInfo", 471 },
    { "UnableToAllocateCacheView", 472 },
    { "UnableToAllocateColorInfo", 473 },
    { "UnableToAllocateDashPattern", 474 },
    { "UnableToAllocateDelegateInfo", 475 },
    { "UnableToAllocateDerivatives", 476 },
    { "UnableToAllocateDrawContext", 477 },
    { "UnableToAllocateDrawInfo", 478 },
    { "UnableToAllocateDrawingWand", 479 },
    { "UnableToAllocateGammaMap", 480 },
    { "UnableToAllocateImage", 481 },
    { "UnableToAllocateImagePixels", 482 },
    { "UnableToAllocateLogInfo", 483 },
    { "UnableToAllocateMagicInfo", 484 },
    { "UnableToAllocateMagickInfo", 485 },
    { "UnableToAllocateMagickMap", 486 },
    { "UnableToAllocateModuleInfo", 487 },
    { "UnableToAllocateMontageInfo", 488 },
    { "UnableToAllocateQuantizeInfo", 489 },
    { "UnableToAllocateRandomKernel", 490 },
    { "UnableToAllocateRegistryInfo", 491 },
    { "UnableToAllocateSemaphoreInfo", 492 },
    { "UnableToAllocateString", 493 },
    { "UnableToAllocateTypeInfo", 494 },
    { "UnableToAllocateWand", 495 },
    { "UnableToAnimateImageSequence", 496 },
    { "UnableToCloneBlobInfo", 497 },
    { "UnableToCloneCacheInfo", 498 },
    { "UnableToCloneImage", 499 },
    { "UnableToCloneImageInfo", 500 },
    { "UnableToConcatenateString", 501 },
    { "UnableToConvertText", 502 },
    { "UnableToCreateColormap", 503 },
    { "UnableToDestroySemaphore", 504 },
    { "UnableToDisplayImage", 505 },
    { "UnableToEscapeString", 506 },
    { "UnableToInitializeSemaphore", 507 },
    { "UnableToInterpretMSLImage", 508 },
    { "UnableToLockSemaphore", 509 },
    { "UnableToObtainRandomEntropy", 510 },
    { "UnableToUnlockSemaphore", 511 },
    { "MemoryAllocationFailed", 512 },
    { "ImageDoesNotContainTheStreamGeometry", 513 },
    { "NoStreamHandlerIsDefined", 514 },
    { "PixelCacheIsNotOpen", 515 },
    { "UnableToAcquirePixelStream", 516 },
    { "UnableToSetPixelStream", 517 },
    { "UnableToSyncPixelStream", 518 },
    { "Default", 519 },
    { "Default", 520 },
    { "FontSubstitutionRequired", 521 },
    { "UnableToGetTypeMetrics", 522 },
    { "UnableToInitializeFreetypeLibrary", 523 },
    { "UnableToReadFont", 524 },
    { "UnrecognizedFontEncoding", 525 },
    { "Default", 526 },
    { "Default", 527 },
    { "InvalidColormapIndex", 528 },
    { "WandAPINotImplemented", 529 },
    { "WandContainsNoImageIndexs", 530 },
    { "WandContainsNoImages", 531 },
    { "ColorIsNotKnownToServer", 532 },
    { "NoWindowWithSpecifiedIDExists", 533 },
    { "StandardColormapIsNotInitialized", 534 },
    { "UnableToConnectToRemoteDisplay", 535 },
    { "UnableToCreateBitmap", 536 },
    { "UnableToCreateColormap", 537 },
    { "UnableToCreatePixmap", 538 },
    { "UnableToCreateProperty", 539 },
    { "UnableToCreateStandardColormap", 540 },
    { "UnableToDisplayImageInfo", 541 },
    { "UnableToGetProperty", 542 },
    { "UnableToGetStandardColormap", 543 },
    { "UnableToGetVisual", 544 },
    { "UnableToGrabMouse", 545 },
    { "UnableToLoadFont", 546 },
    { "UnableToMatchVisualToStandardColormap", 547 },
    { "UnableToOpenXServer", 548 },
    { "UnableToReadXAttributes", 549 },
    { "UnableToReadXWindowImage", 550 },
    { "UnrecognizedColormapType", 551 },
    { "UnrecognizedGravityType", 552 },
    { "UnrecognizedVisualSpecifier", 553 },
    { "UnableToAllocateXHints", 554 },
    { "UnableToCreateCursor", 555 },
    { "UnableToCreateGraphicContext", 556 },
    { "UnableToCreateStandardColormap", 557 },
    { "UnableToCreateTextProperty", 558 },
    { "UnableToCreateXImage", 559 },
    { "UnableToCreateXPixmap", 560 },
    { "UnableToCreateXWindow", 561 },
    { "UnableToDisplayImage", 562 },
    { "UnableToDitherImage", 563 },
    { "UnableToGetPixelInfo", 564 },
    { "UnableToGetVisual", 565 },
    { "UnableToLoadFont", 566 },
    { "UnableToMakeXWindow", 567 },
    { "UnableToOpenXServer", 568 },
    { "UnableToViewFonts", 569 },
    { "UnableToGetVisual", 570 },
    { "UsingDefaultVisual", 571 },
    { 0, 0 }
  };
#endif

#if defined(_INCLUDE_MESSAGE_TABLE_)
static const char *message_dat[] =
  {
    "%1",
    "Unable to create blob",
    "Unable to deduce image format",
    "Unable to obtain current offset",
    "Unable to open file",
    "Unable to read file",
    "Unable to read to offset",
    "Unable to seek to offset",
    "Unable to write blob",
    "Unrecognized image format",
    "default error",
    "default warning",
    "Inconsistent persistent cache depth",
    "Pixel cache dimensions incompatible with image dimensions",
    "Pixel cache is not open",
    "Unable to allocate cache view",
    "Unable to clone cache",
    "Unable to extend cache",
    "Unable to get cache nexus",
    "Unable to get pixels from cache",
    "Unable to open cache",
    "Unable to persist pixel cache",
    "Unable to read pixel cache",
    "Unable to sync cache (check temporary file disk space)",
    "disk allocation failed",
    "Unable to extend pixel cache",
    "default warning",
    "Arithmetic overflow",
    "Colormap size exceeds limit",
    "Colormap type not supported",
    "Colorspace model is not supported",
    "Color type not supported",
    "Compression not valid",
    "Data encoding scheme is not supported",
    "Data storage type is not supported",
    "Delta-PNG is not supported",
    "Division by zero",
    "Encrypted WPG image file not supported",
    "Fractal compression not supported",
    "Image column or row size is not supported",
    "Image does not have a matte channel",
    "Image is not tiles",
    "Image type not supported",
    "Incompatible size of double",
    "Irregular channel geometry not supported",
    "JNG compression is not supported",
    "JPEG compression is not supported",
    "JPEG embedding failed",
    "Location type is not supported",
    "Map storage type is not supported",
    "MSB order not supported bitmap",
    "Multi-dimensional matrices are not supported",
    "Multiple record list not supported",
    "No 8BIM data is available",
    "No APP1 data is available",
    "No bitmap on clipboard",
    "No color profile available",
    "No data returned",
    "No image vector graphics",
    "No IPTC info was found",
    "No IPTC profile available",
    "Number of images is not supported",
    "Only continuous tone picture supported",
    "Only level zero files Supported",
    "PNG compression is not supported",
    "PNG library is too old",
    "RLE compression not supported",
    "Subsampling requires that image width be evenly divisible by two",
    "Unable to copy profile",
    "Unable to create a DC",
    "Unable to create bitmap",
    "Unable to decompress image",
    "Unable to Initialize FPX library",
    "Unable to open blob",
    "Unable to read aspect ratio",
    "Unable to read CIELAB images",
    "Unable to read summary info",
    "Unable to set affine matrix",
    "Unable to set aspect ratio",
    "Unable to set color twist",
    "Unable to set contrast",
    "Unable to set filtering value",
    "Unable to set image comment",
    "Unable to set image title",
    "Unable to set JPEG level",
    "Unable to set region of interest",
    "Unable to set summary info",
    "Unable to translate text",
    "Unable to write MPEG parameters",
    "Unable to write to temporary file",
    "Unable to zip-compress image",
    "Unsupported bits per sample",
    "Unsupported cell type in the matrix",
    "WebP decoding failed: user abort",
    "WebP encoding failed: unknown reason",
    "WebP encoding failed: bad dimension",
    "WebP encoding failed: bad write",
    "WebP encoding failed: bitstream out of memory",
    "WebP encoding failed: File too big (> 4GB)",
    "WebP encoding failed: invalid configuration",
    "WebP encoding failed: null parameter",
    "WebP encoding failed: out of memory",
    "WebP encoding failed: partition 0 overflow (> 512K)",
    "WebP encoding failed: partition overflow (> 16M)",
    "WebP encoding failed: user abort",
    "Invalid WebP configuration parameters supplied",
    "WebP failed: invalid parameter",
    "ZIP compression is not supported",
    "default error",
    "Lossless to lossy JPEG conversion",
    "include element nested too deeply",
    "Registry key lookup failed. Package is not properly installed on this machine.",
    "String token maximum length exceeded",
    "Unable to access configuration file",
    "Unable to access font file",
    "Unable to access log configuration file",
    "Unable to access module file",
    "default error",
    "Unable to change to working directory",
    "Unable to get current working directory",
    "Unable to restore current working directory",
    "default warning",
    "An error has occurred reading from file",
    "An error has occurred writing to file",
    "Colormap exceeded colors limit",
    "Compression not valid",
    "Corrupt image",
    "Image file or blob does not contain any image data",
    "Image file has no scenes",
    "Image type not supported",
    "Improper image header",
    "Insufficient image data in file",
    "Invalid colormap index",
    "invalid file format version",
    "Length and filesize do not match",
    "Missing a required image channel",
    "Negative or zero image size",
    "Non OS2 BMP header size less than 40",
    "Not enough tiles found in level",
    "Static planes value not equal to 1",
    "Subsampling requires that image width be evenly divisible by two",
    "Too much image data in file",
    "Unable to read colormap from dump file",
    "Unable to read color profile",
    "Unable to read extension block",
    "Unable to read generic profile",
    "Unable to read image data",
    "Unable to read image header",
    "Unable to read IPTC profile",
    "Unable to read pixmap from dump file",
    "Unable to read sub image data",
    "Unable to read VID image",
    "Unable to read window name from dump file",
    "Unable to runlength decode image",
    "Unable to uncompress image",
    "Unexpected end-of-file",
    "Unexpected sampling factor",
    "Unknown pattern type",
    "Unrecognized bits per pixel",
    "Unrecognized compression",
    "Unrecognized number of colors",
    "Unrecognized XWD header",
    "Unsupported bits per sample",
    "Unsupported number of planes",
    "Unable to persist key",
    "Compression not valid",
    "Corrupt image (some data returned)",
    "Improper image header",
    "Invalid colormap index",
    "Length and filesize do not match",
    "Negative or zero image size",
    "Non OS2 header size error",
    "Corrupt PCD image, skipping to sync byte",
    "Static planes value not equal to one",
    "Unrecognized bits per pixel",
    "Unrecognized image compression",
    "Delegate failed",
    "Failed to allocate argument list.",
    "Failed to allocate Ghostscript interpreter.",
    "Failed to compute output size",
    "Failed to find Ghostscript (not installed?).",
    "Failed to render file",
    "Failed to scan file",
    "No tag found",
    "Postscript delegate failed",
    "Unable to create image",
    "Unable to create image component",
    "Unable to decode image file",
    "Unable to encode image file",
    "Unable to initialize FPX library",
    "Unable to initialize WMF library",
    "Unable to manage JP2 stream",
    "Unable to write SVG format",
    "WebP library ABI does not match header ABI (build issue!)",
    "default error",
    "default warning",
    "Already pushing pattern definition",
    "drawing recursion detected",
    "text value does not convert to float",
    "text value does not convert to integer",
    "invalid primitive argument",
    "Non-conforming drawing primitive definition",
    "primitive arithmetic overflow",
    "too many coordinates",
    "Unable to print",
    "unbalanced graphic context push-pop",
    "unreasonable gradient image size",
    "vector path truncated",
    "default error",
    "Not a relative URL",
    "Not currently pushing pattern definition",
    "URL not found",
    "Unable to create temporary file",
    "Unable to open file",
    "Unable to write file",
    "default error",
    "default warning",
    "angle is discontinuous",
    "CMYKA image lacks an alpha channel (indexes)",
    "Colorspace color profile mismatch",
    "image colorspace differs",
    "image colorspace mismatch",
    "image difference exceeds limit (%s)",
    "image does not contain resolution",
    "image is not colormapped",
    "image opacity differs",
    "Image sequence is required",
    "image size differs",
    "Invalid colormap index",
    "left and right image sizes differ",
    "no images were found",
    "no images were loaded",
    "no [LOCALE] image attribute",
    "too many cluster",
    "unable to append image",
    "Unable to assign profile",
    "unable to average image",
    "unable to coalesce image",
    "unable to compare images",
    "unable to create image mosaic",
    "unable to create stereo image",
    "unable to deconstruct image sequence",
    "unable to export image pixels",
    "unable to flatten image",
    "Unable to get clip mask",
    "Unable to get composite mask",
    "unable to handle image channel",
    "unable to import image pixels",
    "unable to resize image",
    "unable to segment image",
    "Unable to set clip mask",
    "Unable to set composite mask",
    "unable to shear image",
    "width or height exceeds limit",
    "Unable to persist key",
    "default warning",
    "DPS library is not available",
    "FPX library is not available",
    "FreeType library is not available",
    "JPEG compression library is not available",
    "LCMS encoding not enabled",
    "LZW encoding not enabled",
    "No decode delegate for this image format",
    "No encode delegate for this image format",
    "TIFF library is not available",
    "XML library is not available",
    "X Window library is not available",
    "ZLIB compression library is not available",
    "default error",
    "default warning",
    "Failed to close module",
    "Failed to find symbol",
    "Unable to load module",
    "Unable to register image format",
    "Unrecognized module",
    "Unable to initialize module loader",
    "default warning",
    "default error",
    "default error",
    "User requested termination (via signal)",
    "default warning",
    "bevel width is negative",
    "color separated image required",
    "frame is less than image size",
    "geometry dimensions are zero",
    "geometry does not contain image",
    "hald clut image dimensions are invalid",
    "images are not the same size",
    "size must exceed bevel width",
    "image smaller than kernel width",
    "image smaller than radius",
    "image widths or heights differ",
    "input images already specified",
    "Invalid subimage specification",
    "kernel radius is too small",
    "kernel width must be an odd number",
    "Matrix is not square (%s elements)",
    "Matrix size is out of range",
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
    "Must specify a image name",
    "Must specify image size",
    "No Binary Large OBjects defined",
    "No images defined",
    "Non-zero width and height required",
    "No profile name was given",
    "Null blob argument",
    "Reference image required",
    "Reference is not my type",
    "Region area exceeds implementation limit",
    "Request did not return an image",
    "Stegano image required",
    "Stereo image required",
    "Subimage specification returns no images",
    "Tile is not bounded by image dimensions",
    "Unable to add or remove profile",
    "unable to average image sequence",
    "unable to blur image",
    "unable to chop image",
    "Unable to color matrix image",
    "Unable to constitute image",
    "Unable to convolve image",
    "Unable to edge image",
    "Unable to equalize image",
    "Unable to filter image",
    "unable to format image meta data",
    "Unable to frame image",
    "unable to oil paint image",
    "Unable to paint image",
    "Unable to raise image",
    "Unable to sharpen image",
    "Unable to threshold image",
    "Unable to wave image",
    "Unrecognized attribute",
    "Unrecognized channel type",
    "Unrecognized color",
    "Unrecognized colormap type",
    "Unrecognized image colorspace",
    "Unrecognized command '%s'. Use -help for a usage summary or see manual.",
    "Unrecognized compose operator",
    "Unrecognized dispose method",
    "Unrecognized element",
    "Unrecognized endian type",
    "Unrecognized gravity type",
    "Unrecognized highlight style",
    "Unrecognized image compression",
    "Unrecognized image filter",
    "Unrecognized image format",
    "Unrecognized image mode",
    "Unrecognized image type",
    "Unrecognized intent type",
    "Unrecognized interlace type",
    "Unrecognized list type",
    "Unrecognized error metric",
    "Unrecognized mode type",
    "Unrecognized noise type",
    "Unrecognized operator",
    "Unrecognized option",
    "Unrecognized PerlMagick method",
    "Unrecognized pixel map",
    "Unrecognized preview type",
    "Unrecognized resource type",
    "Unrecognized type",
    "Unrecognized units type",
    "Unrecognized virtual pixel method",
    "Unsupported sampling factor",
    "Improper arguments supplied, please see manual",
    "Invalid colorspace type",
    "Invalid endian type",
    "Invalid image type",
    "Invalid interlace type",
    "Missing an image filename",
    "Option '%s' requires an argument or argument is malformed",
    "No images were loaded",
    "Option length exceeds limit",
    "Request did not return an image",
    "Unable to open XServer",
    "Unable to persist key",
    "Unrecognized colormap type",
    "Unrecognized colorspace type",
    "unrecognized dispose method",
    "Unrecognized endian type",
    "Unrecognized filter type",
    "unrecognized compression type",
    "Unrecognized image type",
    "Unrecognized interlace type",
    "Unrecognized option",
    "Unrecognized resource type",
    "Unrecognized virtual pixel method",
    "Unrecognized color",
    "image expected",
    "image info expected",
    "structure size mismatch",
    "Unable to get registry ID",
    "Unable to locate image",
    "Unable to set registry",
    "default error",
    "default warning",
    "Disk space limit exceeded (see -limit Disk)",
    "Image pixel height limit exceeded (see -limit Height)",
    "Image pixel limit exceeded (see -limit Pixels)",
    "Image pixel width limit exceeded (see -limit Width)",
    "Memory allocation failed",
    "No pixels defined in cache",
    "Pixel cache allocation failed",
    "unable to add ICC Color profile",
    "unable to add generic profile",
    "unable to add IPTC profile",
    "unable to add or remove profile",
    "unable to allocate coefficients",
    "Unable to allocate colormap",
    "unable to allocate ICC profile",
    "Unable to allocate image",
    "unable to allocate string",
    "Unable to annotate image",
    "unable to average image sequence",
    "unable to clone drawing wand",
    "unable to clone image",
    "unable to compute image signature",
    "unable to constitute image",
    "unable to convert font",
    "unable to convert strings to tokens",
    "Unable to create colormap",
    "unable to create color transform",
    "unable to create command widget",
    "unable to create image group",
    "Unable to create image montage",
    "unable to create X window",
    "unable to crop image",
    "unable to despeckle image",
    "unable to determine image class",
    "unable to determine the number of image colors",
    "unable to dither image",
    "unable to draw on image",
    "unable to edge image",
    "unable to emboss image",
    "unable to enhance image",
    "unable to floodfill image",
    "unable to gamma correct image",
    "unable to get best icon size",
    "unable to get from registry",
    "Unable to get package info",
    "unable to level image",
    "unable to magnify image",
    "Unable to manage color",
    "Unable to map image",
    "Unable to map image sequence",
    "unable to median filter image",
    "unable to motion blur image",
    "unable to noise filter image",
    "unable to normalize image",
    "unable to open color profile",
    "unable to quantize image",
    "unable to quantize image sequence",
    "unable to read text chunk",
    "unable to read X image",
    "unable to read X server colormap",
    "unable to resize image",
    "unable to rotate image",
    "unable to sample image",
    "unable to scale image",
    "unable to select image",
    "unable to sharpen image",
    "unable to shave image",
    "unable to shear image",
    "unable to sort image colormap",
    "unable to threshold image",
    "unable to transform colorspace",
    "Memory allocation failed",
    "Semaphore operation failed",
    "unable to allocate ascii85 info",
    "unable to allocate cache info",
    "unable to allocate cache view",
    "unable to allocate color info",
    "unable to allocate dash pattern",
    "unable to allocate delegate info",
    "unable to allocate derivates",
    "unable to allocate draw context",
    "unable to allocate draw info",
    "unable to allocate drawing wand",
    "unable to allocate gamma map",
    "unable to allocate image",
    "unable to allocate image pixels",
    "unable to allocate log info",
    "unable to allocate magic info",
    "unable to allocate magick info",
    "unable to allocate magick map",
    "unable to allocate module info",
    "unable to allocate montage info",
    "unable to allocate quantize info",
    "unable to allocate random kernel",
    "unable to allocate registry info",
    "unable to allocate semaphore info",
    "unable to allocate string",
    "unable to allocate type info",
    "unable to allocate wand",
    "unable to animate image sequence",
    "unable to clone blob info",
    "unable to clone cache info",
    "unable to clone image",
    "unable to clone image info",
    "unable to concatenate string",
    "unable to convert text",
    "unable to create colormap",
    "unable to destroy semaphore",
    "unable to display image",
    "unable to escape string",
    "unable to initialize semaphore",
    "unable to interpret MSL image",
    "unable to lock semaphore",
    "unable to obtain random bytes from operating system",
    "unable to unlock semaphore",
    "Memory allocation failed",
    "image does not contain the stream geometry",
    "no stream handler is defined",
    "Pixel cache is not open",
    "Unable to acquire pixel stream",
    "Unable to set pixel stream",
    "Unable to sync pixel stream",
    "default error",
    "default warning",
    "Font substitution required",
    "Unable to get type metrics",
    "Unable to initialize freetype library",
    "Unable to read font",
    "Unrecognized font encoding",
    "default error",
    "default warning",
    "invalid colormap index `%.1024s",
    "Wand API not implemented `%.1024s",
    "Wand contains no image indices `%.1024s",
    "Wand contains no images `%.1024s",
    "Color is not known to server",
    "No window with specified ID exists",
    "Standard Colormap is not initialized",
    "Unable to connect to remote display",
    "Unable to create bitmap",
    "Unable to create colormap",
    "Unable to create pixmap",
    "Unable to create property",
    "Unable to create standard colormap",
    "Unable to display image info",
    "Unable to get property",
    "Unable to get Standard Colormap",
    "Unable to get visual",
    "Unable to grab mouse",
    "Unable to load font",
    "Unable to match visual to Standard Colormap",
    "Unable to open X server",
    "Unable to read X attributes",
    "Unable to read X window image",
    "Unrecognized colormap type",
    "Unrecognized gravity type",
    "Unrecognized visual specifier",
    "Unable to allocate X hints",
    "Unable to create X cursor",
    "Unable to create graphic context",
    "unable to create standard colormap",
    "Unable to create text property",
    "Unable to create X image",
    "Unable to create X pixmap",
    "Unable to create X window",
    "unable to display image",
    "unable to dither image",
    "Unable to get pixel info",
    "Unable to get visual",
    "Unable to load font",
    "Unable to make X window",
    "Unable to open X server",
    "Unable to view fonts",
    "Unable to get visual",
    "UsingDefaultVisual",
    0
  };
#endif
