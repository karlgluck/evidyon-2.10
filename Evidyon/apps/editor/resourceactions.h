

// most of these should be moved into DC/DCX libraries, or other places in this editor



// candidate for DCX
bool PromptSelectSingleResource(const char* caption, const char* prompt,
                                 const dc::dcGenericResource::Array& resourceOptions,
                                 dc::dcGenericResource** returnedSelection);

// candidate for DCX
bool PromptSelectMultipleResources(const char* caption, const char* prompt,
                                    const dc::dcGenericResource::Array& resourceOptions,
                                          dc::dcGenericResource::Array* selection,
                                    bool initializeSelection = false);