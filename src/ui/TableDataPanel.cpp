#include "TableDataPanel.h"

wxBEGIN_EVENT_TABLE(TableDataPanel, wxPanel)
    // Add event handlers here if necessary
wxEND_EVENT_TABLE()


TableDataPanel::TableDataPanel(wxWindow *parent)
        : wxPanel(parent, wxID_ANY) {

    // Create a grid with 2 columns: Property and Value
    infoGrid = new wxGrid(this, wxID_ANY);

    // Configure the grid
    infoGrid->CreateGrid(0, 2);  // Start with 0 rows, 2 columns
    infoGrid->SetColLabelValue(0, "Property");
    infoGrid->SetColLabelValue(1, "Value");
    infoGrid->EnableEditing(false);

    // Remove row labels (row numbers)
    infoGrid->HideRowLabels();

    // Disable resizing of grid and grid lines
    infoGrid->DisableDragGridSize();
    infoGrid->SetGridLineColour(wxColour(200, 200, 200));  // Optional, for visual effect

    // Use a vertical box sizer for layout and make sure it expands
    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(infoGrid, 1, wxEXPAND | wxALL, 5); // wxEXPAND ensures it fills the parent container
    SetSizer(sizer);

    // Configure word wrapping for the second column
    wxGridCellAttr *attr = new wxGridCellAttr();
    attr->SetOverflow(false);  // Prevent text from overflowing into other cells
    attr->SetAlignment(wxALIGN_LEFT, wxALIGN_TOP);
    attr->SetRenderer(new wxGridCellAutoWrapStringRenderer());  // Use word wrapping renderer
    infoGrid->SetColAttr(1, attr);  // Apply to the "Value" column

    // Make sure the grid resizes columns to fit the panel width dynamically
    Bind(wxEVT_SIZE, &TableDataPanel::OnSize, this);
}


TableDataPanel::~TableDataPanel() {
    // Cleanup (if any)
}


void TableDataPanel::SetData(const std::unordered_map<std::string, std::string> &image_info) {
    // Clear existing rows
    infoGrid->ClearGrid();
    if (infoGrid->GetNumberRows() > 0) {
        infoGrid->DeleteRows(0, infoGrid->GetNumberRows());
    }

    // Add rows to the grid
    for (const auto &info: image_info) {
        infoGrid->AppendRows(1);
        int currentRow = infoGrid->GetNumberRows() - 1;

        infoGrid->SetCellValue(currentRow, 0, info.first);   // Set Property
        infoGrid->SetCellValue(currentRow, 1, info.second);  // Set Value
    }

    infoGrid->AutoSizeColumn(0);
    Layout();
}


void TableDataPanel::OnSize(wxSizeEvent &event) {
    wxSize panelSize = GetClientSize();
    int totalWidth = panelSize.GetWidth() - 10; // subtract padding

    // Automatically size column 0 based on content
    infoGrid->AutoSizeColumn(0);

    // Adjust the size of column 1 based on the remaining space
    int col0Width = infoGrid->GetColSize(0);  // Get the width of the first column
    int col1Width = totalWidth - col0Width;  // Use the remaining space for column 1

    if (col1Width > 0) {
        infoGrid->SetColSize(1, col1Width);
    }

    event.Skip();  // Ensure the event propagates further
}
