#include "stdafx.h"
#pragma hdrstop

#include "RemoteFsTree.h"
// state images
#include "res/file.xpm"
#include "res/file-selected.xpm"
#include "res/folder.xpm"
#include "res/folder-empty.xpm"
#include "res/folder-selected.xpm"
#include "res/folder-selected-partial.xpm"

// initialize data from filesystem item
void RemoteFsTree::FsData::initFrom(const FsItem& item)
{
	m_fsItem = item;
	m_isPath = false;
	m_path.clear();
}

// initialize from path string
void RemoteFsTree::FsData::initFrom(const EsString& path)
{
	m_isPath = true;
	m_path = path;
	memset( &m_fsItem, 0, FsItem_SZE );
}

EsString RemoteFsTree::FsData::getName() const 
{ 
	if( m_isPath ) 
		return m_path; 
	else 
		return wxString(m_fsItem.fname, *wxConvCurrent); 
}

// selected file implementation
//
EsString RemoteFsTree::SelectedFile::getLabel() const
{
	if( m_label.empty() )
	{
		wxFileName path(m_path.c_str());
		size_t cnt = path.GetDirCount();
		if( cnt )
			return path.GetDirs()[cnt-1];
		else
			return m_path;
	}
	else
		return m_label;
}

// remote filesystem tree control implementation
//
IMPLEMENT_DYNAMIC_CLASS(RemoteFsTree, wxTreeCtrl)

RemoteFsTree::RemoteFsTree(wxWindow* parent, wxWindowID id) :
wxTreeCtrl(parent, id, wxDefaultPosition, wxDefaultSize, 
	wxTR_HAS_BUTTONS|wxTR_HIDE_ROOT|wxTR_LINES_AT_ROOT, wxDefaultValidator, wxT("remoteFsTree"))
{
	// set-up state image list, which will be owned by the control
	wxImageList* states = new wxImageList(16, 16);
	wxASSERT(states);
	states->Add( wxBitmap(folder_empty_xpm, wxBITMAP_TYPE_XPM) );
	states->Add( wxBitmap(folder_xpm, wxBITMAP_TYPE_XBM) );
	states->Add( wxBitmap(folder_selected_xpm, wxBITMAP_TYPE_XBM) );
	states->Add( wxBitmap(folder_selected_partial_xpm, wxBITMAP_TYPE_XPM) );
	states->Add( wxBitmap(file_xpm, wxBITMAP_TYPE_XPM) );
	states->Add( wxBitmap(file_selected_xpm, wxBITMAP_TYPE_XPM) );
	AssignImageList(states);
	// create root node
	m_root = AddRoot(wxEmptyString);
	// connect events
	Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( RemoteFsTree::onMouseClick ) );
	Connect( wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler( RemoteFsTree::onKeyDown ) );
}

RemoteFsTree::~RemoteFsTree()
{
	Disconnect( wxEVT_LEFT_DOWN, wxMouseEventHandler( RemoteFsTree::onMouseClick ) );
	Disconnect( wxEVT_COMMAND_TREE_KEY_DOWN, wxTreeEventHandler( RemoteFsTree::onKeyDown ) );
}

void RemoteFsTree::internalGetChildrenSelected(const wxTreeItemId& item, bool& allSelected, bool& noneSelected) const
{
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(item, cookie);
	while( child.IsOk() )
	{
		bool isBold = IsBold(child);
		allSelected &= isBold;
		noneSelected &= !isBold;
		if( HasChildren(child) )
			internalGetChildrenSelected(child, allSelected, noneSelected);

		child = GetNextSibling(child);
	}
}

// return true if all children of this item are fully selected
RemoteFsTree::ChildrenSelectedState RemoteFsTree::getChildrenSelected(const wxTreeItemId& item) const
{
	wxASSERT( item.IsOk() );
	wxCriticalSectionLocker lock(m_cs);
	bool allSelected = true;
	bool noneSelected = true;
	internalGetChildrenSelected(item, allSelected, noneSelected);
	if( allSelected )
		return selectedAll;
	else if( noneSelected )
		return selectedNone;
	else
		return selectedSome;
}

void RemoteFsTree::selectAll(bool select /*= true*/)
{
	wxTreeItemId item = GetRootItem();
	if( item.IsOk() )
	{
		wxTreeItemIdValue cookie;
		item = GetFirstChild(item, cookie);
		while( item.IsOk() )
		{
			selectItem(item, select);
			item = GetNextSibling(item);
		}
	}
}

// select|deselect all children of the item
void RemoteFsTree::selectAllChildren(const wxTreeItemId& item, bool select)
{
	wxASSERT( item.IsOk() );
	wxTreeItemIdValue cookie;
	wxTreeItemId child = GetFirstChild(item, cookie);
	while( child.IsOk() )
	{
		SetItemBold(child, select);
		int idx = GetItemImage(child);
		if( idx <= idxFolderSelectedPartial )
		{
			if( select )
				idx = idxFolderSelected;
			else
				idx = idxFolder;
		}
		else
		{
			if( select )
				idx = idxFileSelected;
			else
				idx = idxFile;
		}
		SetItemImage(child, idx);
		
		if( HasChildren(child) )
			selectAllChildren(child, select);
		child = GetNextSibling(child);
	}
}

// item selection, optionally with children
void RemoteFsTree::selectItem(const wxTreeItemId& item, bool select)
{
	wxCriticalSectionLocker lock(m_cs);
	FsData* data = dynamic_cast<FsData*>(GetItemData(item));
	if( data )
	{
		// if item being selected is empty folder item - do nothing
		if( (data->isPath() || data->isDirectory()) && !HasChildren(item) )
			return;
		SetItemBold(item, select);	
		// if item being selected is not root folder item - set partial selection for 
		// all parent items, else - just select all children
		selectAllChildren(item, select);
		int idx = GetItemImage(item);
		if( idx <= idxFolderSelectedPartial )
		{	
			if( select )
				idx = idxFolderSelected;
			else
				idx = idxFolder;
		}
		else
		{
			if( select )
				idx = idxFileSelected;
			else
				idx = idxFile;
		}
		SetItemImage(item, idx);
		
		// travel up the hierarchy, apply possible selection state changes
		wxTreeItemId curItem = GetItemParent(item);
		while( curItem.IsOk() )
		{
			bool allSelected = true;
			bool noneSelected = true;
			internalGetChildrenSelected(curItem, allSelected, noneSelected);
			if( allSelected )
			{
				idx = idxFolderSelected;
				select = true;
			}
			else if( noneSelected )
			{
				idx = idxFolder;
				select = false;	
			}
			else
			{
				idx = idxFolderSelectedPartial;
				select = true;					
			}
			SetItemBold(curItem, select);	
			SetItemImage(curItem, idx);
			curItem = GetItemParent(curItem);
		}
	}
}

wxTreeItemId RemoteFsTree::appendRootDir(const EsString& dir, const EsString& displayLabel)
{
	wxCriticalSectionLocker lock(m_cs);
	FsData* data = new FsData(dir);
	return AppendItem(m_root, displayLabel.empty() ? data->getName() : displayLabel, idxFolderEmpty, -1, data);
}

wxTreeItemId RemoteFsTree::appendFsItem(const wxTreeItemId& parent, const FsItem& item, const EsString& displayLabel)
{
	wxCriticalSectionLocker lock(m_cs);
	FsData* data = new FsData(item);
	// update parent item's icon, if needed
	SetItemImage(parent, idxFolder);
	return AppendItem(parent, displayLabel.empty() ? data->getName() : displayLabel, 
		data->isDirectory() ? idxFolderEmpty : idxFile, -1, data);
}

EsString RemoteFsTree::getPath(const wxTreeItemId& item) const
{
	wxCriticalSectionLocker lock(m_cs);
	EsString result;
	wxTreeItemId curItem = item;
	
	while( curItem.IsOk() )
	{
		FsData* data = dynamic_cast<FsData*>(GetItemData(curItem));
		if( data )
		{
			if( result.empty() )
				result = data->getName();
			else
				result = data->getName() + wxT("/") + result;
		}
			
		curItem = GetItemParent(curItem);
	}
	
	return result;
}

bool RemoteFsTree::isFolderItem(const wxTreeItemId& item) const
{
	wxCriticalSectionLocker lock(m_cs);
	int idx = GetItemImage(item);
	return -1 != idx && idx <= idxFolderSelectedPartial;
}

RemoteFsTree::FsData RemoteFsTree::getItemData(const wxTreeItemId& item) const
{
	wxCriticalSectionLocker lock(m_cs);
	const FsData* data = dynamic_cast<const FsData*>( GetItemData(item) );
	wxASSERT(data);
	return FsData(*data);
}

// internal selected item enumerator
void RemoteFsTree::enumerateSelected(const wxTreeItemId& parent, RemoteFsTree::SelectedFilesT& out) const
{
	// search all file items & add selected (i.e. bold) ones
	wxTreeItemIdValue cookie;
	wxTreeItemId curItem = GetFirstChild( parent, cookie );
	while( curItem.IsOk() )
	{
		if( IsBold(curItem) )
		{
			if( HasChildren(curItem) )
				enumerateSelected(curItem, out);
			else if( !isFolderItem(curItem) )
			{
				EsString path = getPath(curItem);
				EsString dir = wxFileName(path).GetPath(false);
				out.insert( std::make_pair(dir, SelectedFile(path, 
					*dynamic_cast<FsData*>(GetItemData(curItem)), GetItemText(curItem))) );
			}
		}
					
		curItem = GetNextSibling(curItem);
	}
}

RemoteFsTree::SelectedFilesT RemoteFsTree::getSelectedFiles() const
{
	SelectedFilesT result;
	
	// search all file items & add selected (i.e. bold) ones
	enumerateSelected(m_root, result);
	
	return result;
}

// event handlers
void RemoteFsTree::onMouseClick( wxMouseEvent& evt )
{ 
	int flags = 0;
	wxTreeItemId item = HitTest(evt.GetPosition(), flags);
	if( item.IsOk() && (flags & (wxTREE_HITTEST_ONITEMICON|wxTREE_HITTEST_ONITEMLABEL)) )
		// toggle item selection
		selectItem(item, !IsBold(item));

	evt.Skip();
}

void RemoteFsTree::onKeyDown( wxTreeEvent& evt )
{ 
	if( WXK_SPACE == evt.GetKeyCode() )
	{
		wxTreeItemId item = evt.GetItem();
		if( item.IsOk() )
			// toggle item selection
			selectItem(item, !IsBold(item));		
	}

	evt.Skip();
}