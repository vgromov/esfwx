#ifndef _remote_fs_tree_h_
#define _remote_fs_tree_h_

// remote filesystem tree control class
// implements hub filesystem browsing & item selection capabilities
//
class EKOSF_SHELL_BASE_CLASS RemoteFsTree : public wxTreeCtrl
{
public:
	// item image indexes
	enum {
		idxFolderEmpty,
		idxFolder,
		idxFolderSelected,
		idxFolderSelectedPartial,
		idxFile,
		idxFileSelected
	};
	// children selected state
	enum ChildrenSelectedState {
		selectedAll,
		selectedSome,
		selectedNone
	};

	// specific filesystem tree data class
	class EKOSF_SHELL_BASE_CLASS FsData :	public wxTreeItemData
	{
	public:
		FsData(const EsString& path) { initFrom(path); }
		FsData(const FsItem& item) { initFrom(item); }
		virtual ~FsData() {}

		// get data specifics
		bool isPath() const { return m_isPath; }
		// initialize data from filesystem item
		void initFrom(const FsItem& item);
		// initialize from path string
		void initFrom(const EsString& path);
		// FsItem members access
		size_t getSize() const { return m_fsItem.fsize; }
		wxDateTime getTimeStamp() const { return EsUtilities::DATETIME2wxDateTime( m_fsItem.fts );	}
		bool isReadOnly() const { return FsAttrReadOnly == (m_fsItem.attrs & FsAttrReadOnly); }
		bool isHidden() const { return FsAttrHidden == (m_fsItem.attrs & FsAttrHidden); }
		bool isSystem() const { return FsAttrSystem == (m_fsItem.attrs & FsAttrSystem); }
		bool isVolumeLabel() const { return FsAttrVolumeLabel == (m_fsItem.attrs & FsAttrVolumeLabel); }
		bool isDirectory() const { return FsAttrDirectory == (m_fsItem.attrs & FsAttrDirectory); }
		bool isArchive() const { return FsAttrArchive == (m_fsItem.attrs & FsAttrArchive); }
		EsString getName() const;
		
	protected:
		bool m_isPath;
		EsString m_path;
		FsItem m_fsItem;
	};
	
	class EKOSF_SHELL_BASE_CLASS SelectedFile
	{
	protected:
		SelectedFile(const EsString& path, const FsData& data, const EsString& label = EsString::s_null) :
		m_path(path), m_label(label), m_fsData(data) {}
		
	public:
		const EsString& getPath() const { return m_path; }
		EsString getLabel() const;
		const FsData& getFsData() const { return m_fsData; }
		
	protected:
		EsString m_path;
		EsString m_label;
		FsData m_fsData;
		
		friend class RemoteFsTree;
	};
	
	typedef std::multimap<EsString, SelectedFile> SelectedFilesT;
		
public:
	RemoteFsTree(wxWindow* parent = 0, wxWindowID id = wxID_ANY);
	virtual ~RemoteFsTree();

	// fs browser|selection services
	//
	// return children selection state
	ChildrenSelectedState getChildrenSelected(const wxTreeItemId& item) const;
	// item selection, optionally with children
	void selectItem(const wxTreeItemId& item, bool select);

	// fs items appending-cleanup, access
	wxTreeItemId appendRootDir(const EsString& dir, const EsString& displayLabel = EsString::s_null);
	void cleanAll() { wxCriticalSectionLocker locker(m_cs); DeleteChildren(m_root); }
	void selectAll(bool select = true);
	wxTreeItemId appendFsItem(const wxTreeItemId& parent, const FsItem& item, const EsString& displayLabel = EsString::s_null);
	EsString getPath(const wxTreeItemId& item) const;
	bool isFolderItem(const wxTreeItemId& item) const;
	FsData getItemData(const wxTreeItemId& item) const;
	SelectedFilesT getSelectedFiles() const;

protected:
	// select|deselect all children of the item
	void selectAllChildren(const wxTreeItemId& item, bool select);
	// selection checker internal helper
	void internalGetChildrenSelected(const wxTreeItemId& item, bool& allSelected, bool& noneSelected) const;
	// internal selected item enumerator
	void enumerateSelected(const wxTreeItemId& parent, SelectedFilesT& out) const;
	
	// event handlers
	virtual void onMouseClick( wxMouseEvent& evt );
	virtual void onKeyDown( wxTreeEvent& evt );

protected:
	// root node
	wxTreeItemId m_root;
	// multithread data sharing guard
	mutable wxCriticalSection m_cs;	
	
	DECLARE_DYNAMIC_CLASS(RemoteFsTree)	
};

#endif // _remote_fs_tree_h_
