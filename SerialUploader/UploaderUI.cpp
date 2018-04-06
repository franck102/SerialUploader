#include "UploaderUI.h"

// Returns the only available choice or fails
int8_t UploaderUI::choose(UIChoices &choices)
{
    if (choices.hasNext()) {
        choices.next();
        return (choices.hasNext()) ? -1 : 0;
    } else {
        return -1;
    }
}
